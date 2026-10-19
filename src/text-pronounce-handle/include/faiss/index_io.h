#pragma once

#include "Index.h"

namespace faiss {

// Чтение нативного файла индекса FAISS (.index) с диска
// В оригинальном RVC WebUI файл индекса (added_xxx.index) содержит обученную базу векторов тембра
Index* read_index(const char* fname, int io_flags = 0);

// Запись индекса FAISS в файл
void write_index(const Index* idx, const char* fname);

} // namespace faiss

