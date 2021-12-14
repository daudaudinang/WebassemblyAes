### Thư viện sử dụng:
1. aes.c + aes.h: Thư viện TinyAes để mã hoá và giải mã
2. hextools: Chuyển từ bytearray sang bytearray string để chuyển đi (Do ta không thể return array trong C nên ta cần chuyển về định dạng phù hợp để truyền đi).
3. pkcs7_padding: Do TinyAes chỉ hỗ trợ những chuỗi có độ dài là bộ số của 16, không hỗ trợ thêm bớt padding tự động nên ta cần Library này để thêm và bớt padding theo pkcs7 đối với những plain text độ dài không phải bội số của 16.

### Cách sử dụng:
Ta dùng emscripten để build.

# Cú pháp build:
emcc aes.c -O3 -s WASM=0 -o aes.js -s EXPORTED_FUNCTIONS='["_encryptAES","_decryptAES"]' -s EXPORTED_RUNTIME_METHODS='["cwrap","ccall"]' -s ALLOW_MEMORY_GROWTH=1

Giải thích:
aes.c: File c của chúng ta, chứa 2 function encrypt và decrypt. Nếu muốn đổi IV và Secret Key thì ta sửa trong file này.
O3: Optimize code, -O3 sẽ minimum code sau build để nó gọn và nhẹ nhất
-o aes.js: File đầu ra. Khi chạy lệnh trên ta sẽ có 2 file đầu ra là aes.js và aes.js.mem
EXPORTED_FUNCTIONS: Pick function sẽ được export ra từ file aes.c
EXPORTED_RUNTIME_METHODS: Chọn phương thức chạy, ở lệnh trên ta pick cả cwrap và ccall. Nếu như chỉ cần sử dụng cwrap hoặc chỉ cần sử dụng ccall thì pick cái sử dụng thôi. ()
ALLOW_MEMORY_GROWTH: Cho phép tăng kích thước bộ nhớ.

# Cách call
B1: Import file aes.js và aes.js.mem nhận được sau khi build vào file html của chúng ta
B2: Sử dụng 1 trong 2 method sau để call: 
Đối số theo thứ tự là: "function sử dụng", "kiểu dữ liệu trả về", ["kiểu dữ liệu tham số truyền vào"], "tham số truyền vào"
let x = Module.cwrap("encryptAES","string",["string"])("hahahaha123+/12^*4");
Module.cwrap("decryptAES","string",["string"])(x);
let y = Module.ccall("encryptAES","string",["string"],"hahahaha123+/12^*4");
Module.ccall("decryptAES","string",["string"],y);