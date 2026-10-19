import sys
import argparse
import os

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8")

import torch
import onnx

tools_dir = os.path.dirname(os.path.abspath(__file__))
if tools_dir not in sys.path:
    sys.path.insert(0, tools_dir)

# Импорт класса генератора для ONNX-трассировки из репозитория RVC
try:
    from infer_pack.models_onnx import SynthesizerTrnMsNSFsidM
except ImportError:
    try:
        from infer.lib.infer_pack.models_onnx import SynthesizerTrnMsNSFsidM
    except ImportError:
        try:
            from models_onnx import SynthesizerTrnMsNSFsidM
        except ImportError:
            SynthesizerTrnMsNSFsidM = None


def convert_pth_to_onnx(pth_path: str, onnx_output_path: str, use_fp16: bool = False):
    if SynthesizerTrnMsNSFsidM is None:
        raise ImportError(
            "Не найден модуль SynthesizerTrnMsNSFsidM. Убедитесь, что установлены зависимости RVC: "
            "pip install torch onnx torchaudio"
        )

    print(f"[*] Загрузка чекпоинта: {pth_path}")
    cpt = torch.load(pth_path, map_location="cpu", weights_only=False)

    # Извлекаем конфигурацию архитектуры прямо из .pth файла
    config = list(cpt["config"])
    version = cpt.get("version", "v1")
    f0_mode = cpt.get("f0", 1)

    if f0_mode == 0:
        raise ValueError("Модель обучена без поддержки тона (f0=0). Для интонаций нужна модель с f0=1.")

    # Определяем размерность фонетических векторов (256 для RVC v1, 768 для RVC v2)
    vec_channels = 768 if version == "v2" else 256

    # Последний элемент config[-3] в чекпоинте хранит число спикеров (обычно 109)
    spk_embed_dim = cpt["weight"]["emb_g.weight"].shape[0]
    config[-3] = spk_embed_dim

    # Инициализируем генератор RVC для экспорта
    net_g = SynthesizerTrnMsNSFsidM(*config, version=version, is_half=False)
    net_g.load_state_dict(cpt["weight"], strict=False)
    net_g.eval()

    # Обязательно снимаем weight_norm со всех слоев перед экспортом в ONNX
    net_g.remove_weight_norm()

    # Формируем тестовые входные тензоры (200 фреймов = 2 секунды аудио)
    seq_len = 200
    test_phone = torch.rand(1, seq_len, vec_channels, dtype=torch.float32)
    test_phone_lengths = torch.tensor([seq_len], dtype=torch.int64)
    test_pitch = torch.randint(size=(1, seq_len), low=5, high=255, dtype=torch.int64)
    test_pitchf = torch.rand(1, seq_len, dtype=torch.float32) * 200.0
    test_ds = torch.tensor([0], dtype=torch.int64)
    test_rnd = torch.randn(1, 192, seq_len, dtype=torch.float32)

    input_names = ["phone", "phone_lengths", "pitch", "pitchf", "ds", "rnd"]
    output_names = ["audio"]

    # Задаем динамические оси, чтобы в C++ можно было подавать фразы любой длины
    dynamic_axes = {
        "phone": {1: "seq_len"},
        "pitch": {1: "seq_len"},
        "pitchf": {1: "seq_len"},
        "rnd": {2: "seq_len"},
        "audio": {2: "audio_len"},
    }

    print(f"[*] Экспорт в ONNX (версия RVC: {version}, каналов: {vec_channels})...")
    with torch.no_grad():
        torch.onnx.export(
            net_g,
            (test_phone, test_phone_lengths, test_pitch, test_pitchf, test_ds, test_rnd),
            onnx_output_path,
            input_names=input_names,
            output_names=output_names,
            dynamic_axes=dynamic_axes,
            opset_version=17,
            do_constant_folding=True,
            dynamo=False,
        )

    # Проверка целостности графа и запись метаданных
    sr = cpt.get("sr", 40000)
    if isinstance(sr, str):
        if "48k" in sr or "48000" in sr:
            sr = 48000
        elif "40k" in sr or "40000" in sr:
            sr = 40000
        elif "32k" in sr or "32000" in sr:
            sr = 32000
        else:
            sr = int(sr.replace("k", "000"))
    else:
        sr = int(sr)

    model_onnx = onnx.load(onnx_output_path)
    onnx.checker.check_model(model_onnx)

    # Вшиваем частоту дискретизации и версию в метаданные ONNX файла
    meta_sr = model_onnx.metadata_props.add()
    meta_sr.key = "sample_rate"
    meta_sr.value = str(sr)

    meta_ver = model_onnx.metadata_props.add()
    meta_ver.key = "rvc_version"
    meta_ver.value = str(version)

    onnx.save(model_onnx, onnx_output_path)
    print(f"[+] Метаданные записаны: sample_rate={sr} Гц, rvc_version={version}")
    print(f"[+] Готово! Модель сохранена: {onnx_output_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="RVC .pth to .onnx converter")
    parser.add_argument("--input", "-i", required=True, help="Путь к исходному .pth файлу")
    parser.add_argument("--output", "-o", required=True, help="Путь для сохранения .onnx")
    args = parser.parse_args()

    convert_pth_to_onnx(args.input, args.output)

