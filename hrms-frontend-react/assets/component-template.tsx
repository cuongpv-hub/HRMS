import React, { useState, useEffect } from 'react';
import { apiFetch, formatCurrencyVND, formatLocalTime } from './api-client-template';

// Định nghĩa kiểu dữ liệu cho LeaveRequest
export interface LeaveRequest {
    id: number;
    employee_id: number;
    leave_type: 'annual' | 'sick' | 'unpaid';
    start_time: string; // UTC ISO-8601
    end_time: string;   // UTC ISO-8601
    reason: string;
    status: 'pending' | 'approved' | 'rejected';
    approver_id?: number | null;
    created_at: string;
}

interface LeaveRequestListProps {
    employeeId?: number;
    onSelectRequest?: (request: LeaveRequest) => void;
}

export const LeaveRequestList: React.FC<LeaveRequestListProps> = ({ 
    employeeId,
    onSelectRequest 
}) => {
    const [requests, setRequests] = useState<LeaveRequest[]>([]);
    const [loading, setLoading] = useState<boolean>(true);
    const [error, setError] = useState<string | null>(null);

    useEffect(() => {
        const loadRequests = async () => {
            try {
                setLoading(true);
                setError(null);
                
                // Endpoint động dựa theo prop truyền vào
                const url = employeeId 
                    ? `/leave_requests?employee_id=${employeeId}` 
                    : '/leave_requests';
                
                const data = await apiFetch<LeaveRequest[]>(url);
                setRequests(data);
            } catch (err: any) {
                setError(err.message || 'Không thể tải danh sách đơn nghỉ phép.');
            } finally {
                setLoading(false);
            }
        };

        loadRequests();
    }, [employeeId]);

    if (loading) {
        return (
            <div className="flex items-center justify-center p-8 text-slate-400">
                <span className="animate-spin mr-2">⏳</span> Đang tải dữ liệu...
            </div>
        );
    }

    if (error) {
        return (
            <div className="p-4 bg-red-950/40 border border-red-500/50 text-red-200 rounded-lg text-sm">
                ❌ {error}
            </div>
        );
    }

    return (
        <div className="p-6 bg-slate-900/60 backdrop-blur-md border border-slate-800 rounded-2xl shadow-xl">
            <h2 id="section-title" className="text-xl font-semibold text-slate-100 mb-4 flex items-center gap-2">
                📂 Yêu Cầu Nghỉ Phép
            </h2>

            {requests.length === 0 ? (
                <p className="text-slate-400 text-sm">Không có đơn nghỉ phép nào.</p>
            ) : (
                <div className="overflow-x-auto">
                    <table className="w-full text-left border-collapse text-sm">
                        <thead>
                            <tr className="border-b border-slate-800 text-slate-400 font-medium">
                                <th className="py-3 px-4">Loại phép</th>
                                <th className="py-3 px-4">Bắt đầu (Local)</th>
                                <th className="py-3 px-4">Kết thúc (Local)</th>
                                <th className="py-3 px-4">Trạng thái</th>
                                <th className="py-3 px-4">Hành động</th>
                            </tr>
                        </thead>
                        <tbody>
                            {requests.map((req) => (
                                <tr 
                                    key={req.id} 
                                    className="border-b border-slate-800/50 hover:bg-slate-800/30 transition-colors duration-150 ease-in-out"
                                >
                                    <td className="py-3 px-4 font-semibold text-slate-200 capitalize">
                                        {req.leave_type === 'annual' ? 'Nghỉ năm' : req.leave_type === 'sick' ? 'Nghỉ ốm' : 'Nghỉ không lương'}
                                    </td>
                                    <td className="py-3 px-4 text-slate-300">
                                        {formatLocalTime(req.start_time)}
                                    </td>
                                    <td className="py-3 px-4 text-slate-300">
                                        {formatLocalTime(req.end_time)}
                                    </td>
                                    <td className="py-3 px-4">
                                        <span className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-semibold
                                            ${req.status === 'approved' ? 'bg-emerald-500/10 text-emerald-400 border border-emerald-500/20' : 
                                              req.status === 'rejected' ? 'bg-rose-500/10 text-rose-400 border border-rose-500/20' : 
                                              'bg-amber-500/10 text-amber-400 border border-amber-500/20'}`}
                                        >
                                            {req.status === 'approved' ? 'Đã duyệt' : req.status === 'rejected' ? 'Từ chối' : 'Chờ duyệt'}
                                        </span>
                                    </td>
                                    <td className="py-3 px-4">
                                        <button 
                                            id={`btn-view-${req.id}`}
                                            className="px-3 py-1.5 bg-indigo-600 hover:bg-indigo-500 active:scale-95 text-white font-medium rounded-lg text-xs transition-all duration-150"
                                            onClick={() => onSelectRequest?.(req)}
                                        >
                                            Chi tiết
                                        </button>
                                    </td>
                                </tr>
                            ))}
                        </tbody>
                    </table>
                </div>
            )}
        </div>
    );
};
