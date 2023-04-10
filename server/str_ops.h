//
// Created by navin on 4/10/23.
//

#ifndef STR_OPS_H
#define STR_OPS_H

#include <stdbool.h>
#include <stdio.h>

bool has_new_line(char* str, size_t size);
bool write_to_file(FILE* fp, char* data, size_t data_len);
bool read_complete_file(FILE* fp, int sock_fd);

#endif //STR_OPS_H
