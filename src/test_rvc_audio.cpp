#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "text-pronounce-handle.hpp"

int main() {
    std::cout << "=== RVC Audio Pipeline Standalone Test ===\n";

    RvcPipelineConfig config = {
        .models = {
            .contentvec_path    = "models/vec-768-layer-12.onnx",
            .rmvpe_path         = "models/rmvpe.onnx",
            .rvc_generator_path = "models/GawrGura_Sing.onnx",
            .faiss_index_path   = "models/GawrGura_Sing_v2.index"
        },
        .gpu_device_id    = 0,
        .index_rate       = 0.75f,
        .f0_up_key        = 12, // +12 semitones (1 octave up) for female anime voice Gawr Gura
        .speaker_id       = 0,
        .intra_op_threads = 4,
        .use_gpu          = true
    };

    RvcPipelineContext ctx{};
    std::cout << "[Test] Initializing RVC pipeline...\n";
    if (!rvc_pipeline_init(&ctx, &config)) {
        std::cerr << "[Test] FAILED to initialize RVC pipeline!\n";
        return 1;
    }
    std::cout << "[Test] Context status: " << ctx.status_message << "\n";
    std::cout << "[Test] Output sample rate: " << ctx.output_sample_rate << " Hz\n";

    const char* test_wav = "models/sample_speech_16k.wav";
    std::cout << "[Test] Decoding test file: " << test_wav << "\n";
    RvcAudioBuffer in_audio{};
    if (!rvc_audio_decode_file_to_16k(test_wav, &in_audio)) {
        std::cerr << "[Test] FAILED to decode audio file!\n";
        return 1;
    }
    std::cout << "[Test] Decoded " << in_audio.count << " samples at " << in_audio.sample_rate << " Hz\n";

    std::cout << "[Test] Running RVC voice conversion...\n";
    auto start_conv = std::chrono::high_resolution_clock::now();
    RvcAudioBuffer out_audio{};
    if (!rvc_pipeline_convert_voice(&ctx, in_audio.samples, in_audio.count, &out_audio)) {
        std::cerr << "[Test] FAILED voice conversion!\n";
        return 1;
    }
    auto end_conv = std::chrono::high_resolution_clock::now();
    double conv_ms = std::chrono::duration<double, std::milli>(end_conv - start_conv).count();
    std::cout << "[Test] Voice converted in " << conv_ms << " ms! (" << out_audio.count << " samples at " << out_audio.sample_rate << " Hz)\n";

    std::cout << "[Test] Playing converted voice audio through miniaudio WASAPI...\n";
    if (!rvc_pipeline_play_converted(&ctx, &out_audio, true)) {
        std::cerr << "[Test] FAILED to start audio playback!\n";
        return 1;
    }

    std::cout << "[Test] Audio playback finished successfully!\n";

    rvc_pipeline_free_buffer(&in_audio);
    rvc_pipeline_free_buffer(&out_audio);
    rvc_pipeline_destroy(&ctx);

    std::cout << "=== Standalone Test Completed Successfully ===\n";
    return 0;
}

