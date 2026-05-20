/**
 * HRMS API Client Template & Data Formatting Helpers
 */

const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:18080/api';

// Giả lập lấy Token quyền từ hệ thống quản lý State (VD: Redux/Context/LocalStorage)
function getAuthToken(): string {
    // Luôn điền các Scope quyền cần thiết cho thao tác kiểm thử API
    return 'READ_EMPLOYEE WRITE_EMPLOYEE READ_SALARY WRITE_SALARY READ_LEAVE WRITE_LEAVE';
}

/**
 * Hàm gọi API chung (fetch helper)
 * @template T Kiểu dữ liệu mong đợi trả về từ API
 */
export async function apiFetch<T>(endpoint: string, options: RequestInit = {}): Promise<T> {
    const token = getAuthToken();
    
    const headers = new Headers(options.headers);
    headers.set('Content-Type', 'application/json');
    if (token) {
        headers.set('Authorization', `Bearer ${token}`);
    }

    const response = await fetch(`${API_BASE_URL}${endpoint}`, {
        ...options,
        headers
    });

    if (!response.ok) {
        if (response.status === 403) {
            throw new Error('403 Forbidden: Quyền hạn hiện tại không đủ để thực hiện hành động này.');
        }
        if (response.status === 404) {
            throw new Error('404 Not Found: Tài nguyên không tồn tại hoặc đã bị xóa mềm.');
        }
        const errorText = await response.text();
        throw new Error(errorText || `Yêu cầu thất bại với mã lỗi: ${response.status}`);
    }

    // Trả về JSON nếu có body
    if (response.status === 204) {
        return {} as T;
    }
    return response.json() as Promise<T>;
}

/**
 * GOTCHA TIỀN TỆ: Chuyển đổi mức lương lưu dạng Integer/Cents ở CSDL sang định dạng hiển thị VND.
 * @param amount Mức lương nhận từ API (kiểu int64_t)
 */
export function formatCurrencyVND(amount: number | null | undefined): string {
    if (amount === null || amount === undefined) {
        return '---đ';
    }
    
    // GOTCHA: Tuyệt đối KHÔNG sử dụng float để tính toán lương gốc nhằm tránh sai số.
    return new Intl.NumberFormat('vi-VN', {
        style: 'currency',
        currency: 'VND',
        maximumFractionDigits: 0
    }).format(amount);
}

/**
 * GOTCHA MÚI GIỜ: Chuyển đổi chuỗi UTC ISO-8601 từ Database sang múi giờ Việt Nam (GMT+7)
 * @param utcString Chuỗi ngày giờ dạng UTC (VD: "2026-05-18T08:00:00Z")
 */
export function formatLocalTime(utcString: string | null | undefined): string {
    if (!utcString) {
        return '---';
    }

    try {
        const date = new Date(utcString);
        return new Intl.DateTimeFormat('vi-VN', {
            timeZone: 'Asia/Ho_Chi_Minh',
            year: 'numeric',
            month: '2-digit',
            day: '2-digit',
            hour: '2-digit',
            minute: '2-digit',
            hour12: false
        }).format(date);
    } catch (e) {
        // Trả về chuỗi thô ban đầu nếu lỗi parse để tránh crash UI
        return utcString;
    }
}
