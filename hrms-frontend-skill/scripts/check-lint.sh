#!/bin/bash
# Script kiểm tra lỗi cú pháp, định dạng bằng ESLint và Prettier

echo "=== [LINT] Bắt đầu quét lỗi mã nguồn Frontend... ==="

# Kiểm tra xem npm đã được cài đặt chưa
if ! command -v npm &> /dev/null; then
    echo "❌ Lỗi: Node.js / npm chưa được cài đặt trên hệ thống."
    exit 1
fi

# Chạy ESLint để quét lỗi logic/code style
echo "🔍 Đang chạy ESLint..."
npx eslint "src/**/*.{ts,tsx}" --quiet
ESLINT_STATUS=$?

# Chạy Prettier để kiểm tra định dạng
echo "🔍 Đang chạy Prettier (Kiểm tra định dạng)..."
npx prettier --check "src/**/*.{ts,tsx,css,json}"
PRETTIER_STATUS=$?

if [ $ESLINT_STATUS -eq 0 ] && [ $PRETTIER_STATUS -eq 0 ]; then
    echo "✅ [SUCCESS] Mã nguồn sạch sẽ! Không phát hiện lỗi linter hay định dạng nào."
    exit 0
else
    echo "❌ [FAILURE] Phát hiện lỗi mã nguồn hoặc lỗi định dạng. Vui lòng chạy 'npx prettier --write .' để tự động sửa."
    exit 1
fi
