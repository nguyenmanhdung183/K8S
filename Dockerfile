# Sử dụng image cơ bản nhẹ
FROM alpine:latest

# Cài đặt gcc và musl-dev (cần thiết để biên dịch C++)
RUN apk add --no-cache g++ libc-dev

# Thiết lập thư mục làm việc
WORKDIR /app

# Sao chép mã nguồn vào container
COPY server.cpp .

# Biên dịch server
RUN g++ -o server server.cpp -pthread

# Mở cổng cho server
EXPOSE 8080

# Chạy server
CMD ["./server"]
