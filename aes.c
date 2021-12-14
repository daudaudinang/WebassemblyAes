#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
// #include <emscripten/emscripten.h>

#define CBC 1

#include "Library/aes.c"
#include "Library/pkcs7_padding.c"
#include "Library/hextools.c"
const uint8_t key[] = "ec75109d-bfe2-48";
const uint8_t iv[] = "KCap6JeLif31Q9xs";

char *encryptAES(char* message){

    // Kiểm tra chuỗi, thêm padding nếu message k phải bội số của 16 (Do thằng tiny-aes không có chức năng thêm padding nên ta cần dùng pkcs7_padding)
    int oldLen = strlen(message);
    int newLen = oldLen;
    if(oldLen % 16){
        newLen += 16 - (oldLen % 16);
    }

    uint8_t str[newLen];

    memset(str, 0, newLen);

    // Fill full data từ message vào mảng mới đã
    for(int i = 0; i < oldLen; i++){
        str[i] = (uint8_t) message[i];
    }

    int messagePad = pkcs7_padding_pad_buffer(str, oldLen, sizeof(str), 16);

    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, str, newLen);

    // Chuyển data từ bin sang hex
    char *encryptedMessage = bin2hex(str, sizeof str);

    return encryptedMessage;
}

char *decryptAES(char* message){
    uint8_t str[strlen(message)/2];
    memset(str, 0, strlen(message)/2);

    for (int i = 0; i < sizeof(str); i++) {
        sscanf(message + 2*i, "%2hhx", &str[i]);
    }

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, str, sizeof(str));

    // Sau khi giải mã xong ta cần bỏ đi phần padding đã thêm vào trước khi encrypt để đưa ra được kết quả chính xác như ban đầu
    size_t actualDataLength = pkcs7_padding_data_length(str, sizeof(str), 16);

    // Tạo 1 buffer tạm để lưu giá trị của chuỗi sau khi đã cắt đi phần padding
    uint8_t tempMessage[actualDataLength + 1];    
    memset(tempMessage, 0, actualDataLength);

    // Nhớ thêm '\0' vào cuối buffer để nó xác định điểm kết thúc của buffer, tránh trường hợp chuyển sang char* bị lỗi
    tempMessage[actualDataLength] = '\0';

    for(int i = 0; i < actualDataLength; i++){
        tempMessage[i] = str[i];
    }

    char* decryptedMessage = (char*) tempMessage;
    return decryptedMessage;
}

// int main(void)
// {
//     char *x = encryptAES("hahahaha1234hashausasw1w1212jajasa");
//     printf("%s\n", x);

//     char *y = decryptAES(x);
//     printf("%s\n", y);

//     return 0;
// }