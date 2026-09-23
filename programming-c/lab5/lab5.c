#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALPHABET_LENGTH 256
#define ONE_BYTE_SIZE 8
#define MAX_BUFFER_LENGTH 1000000

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct Node {
    uchar symbol;
    uint frequency;
    struct Node *left, *right;
} Node;

typedef struct PriorityQueue {
    Node *nodes_array[MAX_ALPHABET_LENGTH];
    int start;
    int end;
} PriorityQueue;

typedef struct BitBuffer {
    uchar *buffer;
    uint buffer_size;
    uint byte_counter;
    uint bit_counter;
} BitBuffer;

Node *CreateNode(uchar symbol, uint frequency, Node *left, Node *right) {
    Node *node = malloc(sizeof(Node));
    if (!node) {return NULL;}
    node->symbol = symbol;
    node->frequency = frequency;
    node->left = left;
    node->right = right;
    return node;
}

void FreeHuffmanTree(Node **root) {
    if (!root|| !*root) {return;}
    if ((*root)->left != NULL) {
        FreeHuffmanTree((&(*root)->left));
        FreeHuffmanTree(&((*root)->right));
    }
    free((*root));
    (*root) = NULL;
}

PriorityQueue *CreatePriorityQueue(void) {
    PriorityQueue *queue = malloc(sizeof(PriorityQueue));
    if (!queue) {return NULL;}
    queue->start = queue->end = -1;
    return queue;
}

void InsertNodeIntoQueue(PriorityQueue *queue, Node *node) {
    if (!queue || !node) {return;}
    if (queue->start == -1 && queue->end == -1) {
        queue->start++;
        queue->end++;
        queue->nodes_array[queue->start] = node;
        return;
    }
    int i = 0;
    for (i = 0; i <= queue->end; i++) {
        if (node->frequency >= queue->nodes_array[i]->frequency) {
            for (int j = queue->end + 1; j > i; j--) {
                queue->nodes_array[j] = queue->nodes_array[j - 1];
            }
            queue->nodes_array[i] = node;
            queue->end++;
            return;
        }
    }
    queue->end++;
    queue->nodes_array[i] = node;
}

Node *GetLeastNode(PriorityQueue *queue) {
    if (!queue|| queue->end < 0) {
        return NULL;
    }
    return queue->nodes_array[queue->end--];
}

long CalculateByteFrequencies(uint *symbol_frequencies, FILE *file) {
    uchar current_symbol;
    long count = 0;
    for (int i = 0; i < MAX_ALPHABET_LENGTH; i++) {
        symbol_frequencies[i] = 0;
    }
    while (fread(&current_symbol, 1, 1, file)) {
        symbol_frequencies[(int)current_symbol]++;
        count++;
    }
    fseek(file, 1, SEEK_SET);
    return count;
}

Node *BuildHuffmanTree(uint *symbol_frequencies) {
    PriorityQueue *queue = CreatePriorityQueue();
    if (!queue) {return NULL;}
    for (int i = 0; i < MAX_ALPHABET_LENGTH; i++) {
        if (symbol_frequencies[i]) {
            InsertNodeIntoQueue(queue, CreateNode(i, symbol_frequencies[i], 0, 0));
        }
    }
    while (queue->end - queue->start > 0) {
        Node *first_node = GetLeastNode(queue), *second_node = GetLeastNode(queue);
        if (!first_node|| !second_node) {
            free(queue);
            return NULL;
        }
        InsertNodeIntoQueue(queue, CreateNode(0, first_node->frequency + second_node->frequency, first_node, second_node));
    }
    Node *root = GetLeastNode(queue);
    free(queue);
    return root;
}

void BuildHuffmanCodes(Node *root, char **codes, char *current_code) {
    const size_t length = strlen(current_code);
    if (!root->left) {
        codes[root->symbol] = (char*)malloc(length + 1);
        if (!codes[root->symbol]) {return;}
        strcpy(codes[root->symbol], current_code);
    }
    else {
        current_code[length] = '0';
        current_code[length + 1] = '\0';
        if (root->left) {
            BuildHuffmanCodes(root->left, codes, current_code);
        }
        current_code[length] = '1';
        current_code[length + 1] = '\0';
        if (root->right) {
            BuildHuffmanCodes(root->right, codes, current_code);
        }
    }
}

void FreeHuffmanCodes(char **codes) {
    for (int i = 0; i < MAX_ALPHABET_LENGTH; i++) {
        if (codes[i]) {
            free(codes[i]);
        }
    }
}

void InitializeBitBuffer(BitBuffer *bit_buffer, uint buffer_size) {
    bit_buffer->buffer_size = buffer_size;
    bit_buffer->bit_counter = 8;
    bit_buffer->byte_counter = 0;
    bit_buffer->buffer = (uchar*)malloc(sizeof(uchar) * buffer_size);
    if (!bit_buffer->buffer) {return;}
    for (uint i = 0; i < buffer_size; i++) {
        bit_buffer->buffer[i] = 0;
    }
}

void WriteByte(FILE *file, BitBuffer *bit_buffer, uchar bit) {
    bit_buffer->buffer[bit_buffer->byte_counter] |= ((bit - '0') << (bit_buffer->bit_counter - 1));
    bit_buffer->bit_counter--;
    if (bit_buffer->bit_counter == 0) {
        fwrite(&bit_buffer->buffer[bit_buffer->byte_counter], sizeof(uchar), 1, file);
        bit_buffer->bit_counter = 8;
        bit_buffer->buffer[bit_buffer->byte_counter] = 0;
    }
}

int ReadBit(BitBuffer *bit_buffer, uchar *bit) {
    if (bit_buffer->byte_counter >= bit_buffer->buffer_size) {
        return 1;
    }
    *bit = (bit_buffer->buffer[bit_buffer->byte_counter] >> (bit_buffer->bit_counter - 1)) & 1;
    bit_buffer->bit_counter--;
    if (bit_buffer->bit_counter == 0) {
        bit_buffer->byte_counter++;
        bit_buffer->bit_counter = 8;
    }
    return 0;
}

void ReadByte(BitBuffer *bit_buffer, uchar *byte) {
    *byte = bit_buffer->buffer[bit_buffer->byte_counter] << (8 - bit_buffer->bit_counter);
    bit_buffer->byte_counter++;
    *byte |= bit_buffer->buffer[bit_buffer->byte_counter] >> bit_buffer->bit_counter;
}

void WriteSymbol(FILE *out, BitBuffer *bit_buffer, const char *code) {
    for (uint i = 0; i < strlen(code); i++) {
        WriteByte(out, bit_buffer, code[i]);
    }
}

void FlushDataBuffer(FILE *out, BitBuffer *bit_buffer) {
    if (bit_buffer->bit_counter != ONE_BYTE_SIZE) {
        fwrite(&(bit_buffer->buffer[bit_buffer->byte_counter]), 1, 1, out);
    }
}

uchar ReadSymbol(FILE *file, Node *root, BitBuffer *bit_buffer) {
    if (root->left) {
        uchar bit = 0;
        if (ReadBit(bit_buffer, &bit)) {
            if (!fread(bit_buffer->buffer, 1, MAX_BUFFER_LENGTH, file)) {
                return 0;
            }
            bit_buffer->byte_counter = 0;
            ReadBit(bit_buffer, &bit);
        }
        if (bit) {
            return ReadSymbol(file, root->right, bit_buffer);
        }
        else {
            return ReadSymbol(file, root->left, bit_buffer);
        }
    }
    else {
        return root->symbol;
    }
}

void PackHuffmanTree(FILE *file, BitBuffer *bit_buffer, Node *root) {
    if (!root->left) {
        WriteByte(file, bit_buffer, '0');
        for (int i = 0; i < ONE_BYTE_SIZE; i++) {
            WriteByte(file, bit_buffer, ((root->symbol) >> (ONE_BYTE_SIZE - 1 - i) & 1) + '0');
        }
    }
    else {
        WriteByte(file, bit_buffer, '1');
        PackHuffmanTree(file, bit_buffer, root->left);
        PackHuffmanTree(file, bit_buffer, root->right);
    }
}

void UnpackHuffmanTree(BitBuffer *bit_buffer, Node **root) {
    *root = (Node*)malloc(sizeof(Node));
    if (!*root) {return;}
    (*root)->frequency = 0;
    uchar bit = 0;
    ReadBit(bit_buffer, &bit);
    if (bit) {
        (*root)->symbol = 0;
        UnpackHuffmanTree(bit_buffer, &((*root)->left));
        UnpackHuffmanTree(bit_buffer, &((*root)->right));
    }
    else {
        (*root)->left = 0;
        (*root)->right = 0;
        uchar symbol;
        ReadByte(bit_buffer, &symbol);
        (*root)->symbol = symbol;
    }
}

int Encode(FILE *in, FILE *out) {
    uint table_freguency[MAX_ALPHABET_LENGTH];
    const long text_length = CalculateByteFrequencies(table_freguency, in);
    if (!text_length) {return 0;}
    Node *root = BuildHuffmanTree(table_freguency);
    if (!root) {return 0;}
    char* codes[MAX_ALPHABET_LENGTH] = {NULL};
    char current[MAX_ALPHABET_LENGTH] = {0};
    BuildHuffmanCodes(root, codes, current);
    fwrite(&(text_length), 4, 1, out);
    BitBuffer bit_buffer;
    InitializeBitBuffer(&bit_buffer, 1);
    if (!bit_buffer.buffer) {
        FreeHuffmanTree(&root);
        FreeHuffmanCodes(codes);
        return 0;
    }
    PackHuffmanTree(out, &bit_buffer, root);
    for (long i = 0; i < text_length; i++) {
        uchar symbol;
        if (fread(&symbol, 1, 1, in) != 1) {
            free(bit_buffer.buffer);
            FreeHuffmanTree(&root);
            FreeHuffmanCodes(codes);
            return 0;
        }
        WriteSymbol(out, &bit_buffer, codes[symbol]);
    }
    FlushDataBuffer(out, &bit_buffer);
    free(bit_buffer.buffer);
    FreeHuffmanTree(&root);
    FreeHuffmanCodes(codes);
    return 1;
}

int Decode(FILE *in, FILE *out) {
    Node *root;
    BitBuffer bit_buffer;
    InitializeBitBuffer(&bit_buffer, MAX_BUFFER_LENGTH);
    if (!bit_buffer.buffer) {return 0;}
    uint encoded_char_number = 0;
    if (!fread(&encoded_char_number, 4, 1, in)) {
        free(bit_buffer.buffer);
        return 0;
    }
    if (!fread(bit_buffer.buffer, 1, MAX_BUFFER_LENGTH, in)) {
        free(bit_buffer.buffer);
        return 0;
    }
    UnpackHuffmanTree(&bit_buffer, &root);
    if (!root) {
        free(bit_buffer.buffer);
        return 0;
    }
    for (unsigned long i = 0; i < encoded_char_number; i++) {
        uchar symbol = ReadSymbol(in, root, &bit_buffer);
        fwrite(&symbol, 1, 1, out);
    }
    free(bit_buffer.buffer);
    FreeHuffmanTree(&root);
    return 1; 
}

int main(int argc, char** argv) {
    if (argc==1){
        uchar symbol;
        FILE *in = fopen("in.txt", "rb");
        FILE *out = fopen("out.txt", "wb");
        if (!in || !out) {
            if (in) {fclose(in);}
            if (out) {fclose(out);}
            return 1;
        }
        if (fread(&symbol, 1, 1, in) != 1) {
            fclose(in);
            fclose(out);
            return 1;
        }
        if (symbol == 'c') {Encode(in, out);}
        else {Decode(in, out);}
        fclose(in);
        fclose(out);
        return 0;
    }
    else if (argc == 4) {
        // Режим командной строки: ./program c input.txt output.txt
        const char *command = argv[1];
        const char *input_path = argv[2];
        const char *output_path = argv[3];

        FILE *in = fopen(input_path, "rb");
        FILE *out = fopen(output_path, "wb");
        if (!in || !out) {
            if (in) fclose(in);
            if (out) fclose(out);
            printf("Ошибка открытия файлов!\n");
            return 1;
        }

        // Здесь делаем ту же логику, что и в режиме argc == 1
        // Только вручную имитируем первый байт команды
        FILE *temp_in = tmpfile(); // временный файл
        if (!temp_in) {
            fclose(in);
            fclose(out);
            return 1;
        }

        // Вставляем команду в начало
        if (strcmp(command, "c") == 0) {
            fputc('c', temp_in);
        } else if (strcmp(command, "d") == 0) {
            fputc('d', temp_in);
        } else {
            printf("Неизвестная команда: %s\n", command);
            fclose(in);
            fclose(out);
            fclose(temp_in);
            return 1;
        }

        // Копируем остальное содержимое файла
        uchar buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
            fwrite(buffer, 1, bytes, temp_in);
        }

        rewind(temp_in);  // возвращаемся в начало временного файла

        uchar symbol;
        if (fread(&symbol, 1, 1, temp_in) != 1) {
            fclose(in);
            fclose(out);
            fclose(temp_in);
            return 1;
        }

        int result = (symbol == 'c') ? Encode(temp_in, out) : Decode(temp_in, out);

        fclose(in);
        fclose(out);
        fclose(temp_in);
        return !result;
    }

    else {
        printf("Использование:\n");
        printf("  %s             — команда (c/d) и данные в in.txt, вывод в out.txt\n", argv[0]);
        printf("  %s c|d input output — команда и файлы через аргументы\n", argv[0]);
        return 1;
    }
}

