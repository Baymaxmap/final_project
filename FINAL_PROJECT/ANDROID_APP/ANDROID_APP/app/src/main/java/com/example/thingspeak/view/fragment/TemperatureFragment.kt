package com.example.thingspeak.view.fragment

import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.annotation.RequiresApi
import androidx.fragment.app.Fragment
import com.example.thingspeak.R
import com.example.thingspeak.viewmodel.ThingSpeakViewModel
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import com.github.mikephil.charting.formatter.IndexAxisValueFormatter
import androidx.fragment.app.activityViewModels
import com.example.thingspeak.model.ThingSpeakData
import com.example.thingspeak.view.markerview.CustomMarkerView
import java.time.ZoneId
import java.time.ZonedDateTime
import java.time.format.DateTimeFormatter
import kotlinx.coroutines.*


class TemperatureFragment : Fragment() {

    private lateinit var lineChart: LineChart // Biểu đồ LineChart
    private val viewModel: ThingSpeakViewModel by activityViewModels() // ViewModel dùng chung với Activity
    private val handler = Handler(Looper.getMainLooper()) // Handler để chạy định kỳ
    private lateinit var updateRunnable: Runnable // Runnable cho việc tự động cập nhật

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate giao diện của Fragment
        return inflater.inflate(R.layout.fragment_temperature, container, false)
    }

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        lineChart = view.findViewById(R.id.lineChartTemp) // Kết nối LineChart từ giao diện

        // Quan sát dữ liệu từ ViewModel
        viewModel.data.observe(viewLifecycleOwner) { dataList ->
            updateChart(dataList) // Cập nhật biểu đồ mỗi khi dữ liệu thay đổi
        }

        // Cài đặt Runnable để tự động cập nhật dữ liệu sau mỗi 10 giây
        updateRunnable = Runnable {
            viewModel.fetchAllFields("MI60P3HS5SH5CEWE", results = 100) // Gọi API để lấy dữ liệu mới
            handler.postDelayed(updateRunnable, 10000) // Tiếp tục cập nhật sau 10 giây
        }
        handler.post(updateRunnable) // Bắt đầu chu kỳ cập nhật tự động
    }

    override fun onDestroyView() {
        super.onDestroyView()
        handler.removeCallbacks(updateRunnable) // Xóa các callback để tránh rò rỉ bộ nhớ
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun updateChart(dataList: List<ThingSpeakData>) {
        // Tạo danh sách Entry cho biểu đồ
        val entries = dataList.mapIndexed { index, data ->
            val xValue = index.toFloat()
            val yValue = data.field1?.toFloat() ?: 0f
            Entry(xValue, yValue)
        }

        // Cài đặt LineDataSet để tùy chỉnh giao diện biểu đồ
        val dataSet = LineDataSet(entries, "Temperature").apply {
            color = Color.rgb(255, 87, 34) // Màu của đường biểu đồ
            valueTextColor = Color.DKGRAY // Màu chữ hiển thị trên các điểm
            lineWidth = 2f // Độ dày của đường
            setCircleColor(Color.BLUE) // Màu của các điểm
            circleRadius = 0f // Ẩn vòng tròn tại các điểm
            mode = LineDataSet.Mode.LINEAR // Đường thẳng
            setDrawFilled(true) // Vẽ nền bên dưới đường
            setGradientColor(Color.rgb(255, 183, 77), Color.rgb(255, 87, 34)) // Gradient cho nền
            fillAlpha = 80 // Độ trong suốt của nền
            setDrawValues(false) // Tắt hiển thị chữ tại các điểm
            setDrawCircles(false) // Tắt hiển thị vòng tròn tại các điểm
        }

        // Gán dữ liệu vào biểu đồ
        val lineData = LineData(dataSet)
        lineChart.data = lineData

        // Xử lý timestamps với múi giờ Việt Nam
        val shortFormatter = DateTimeFormatter.ofPattern("HH:mm") // Hiển thị giờ:phút
        val fullFormatter = DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm") // Định dạng đầy đủ cho MarkerView
        val shortTimestamps = dataList.mapNotNull { data ->
            try {
                val utcDateTime = ZonedDateTime.parse(data.created_at) // Parse thời gian từ UTC
                val localDateTime = utcDateTime.withZoneSameInstant(ZoneId.of("Asia/Ho_Chi_Minh")) // Chuyển sang giờ Việt Nam
                localDateTime.format(shortFormatter) // Chỉ định dạng HH:mm cho trục X
            } catch (e: Exception) {
                null // Bỏ qua các giá trị không hợp lệ
            }
        }
        val fullTimestamps = dataList.mapNotNull { data ->
            try {
                val utcDateTime = ZonedDateTime.parse(data.created_at)
                val localDateTime = utcDateTime.withZoneSameInstant(ZoneId.of("Asia/Ho_Chi_Minh"))
                localDateTime.format(fullFormatter) // Định dạng đầy đủ cho MarkerView
            } catch (e: Exception) {
                null
            }
        }

        // Cấu hình trục X cho biểu đồ
        val xAxis = lineChart.xAxis
        xAxis.position = XAxis.XAxisPosition.BOTTOM // Đặt trục X ở dưới
        xAxis.valueFormatter = IndexAxisValueFormatter(shortTimestamps) // Hiển thị thời gian theo định dạng
        xAxis.textColor = Color.WHITE // Đổi màu chữ của trục X
        xAxis.granularity = 5f // Hiển thị nhãn mỗi 5 giá trị
        xAxis.labelCount = 6 // Giới hạn số nhãn hiển thị
        xAxis.labelRotationAngle = -30f // Xoay nhãn để dễ đọc
        xAxis.gridColor = Color.LTGRAY // Màu lưới trên trục X

        // Cấu hình trục Y cho biểu đồ
        val yAxisLeft = lineChart.axisLeft
        yAxisLeft.textColor = Color.WHITE // Đổi màu chữ của trục Y (bên trái)
        yAxisLeft.gridColor = Color.GRAY // Màu lưới trên trục Y
        yAxisLeft.gridLineWidth = 1.5f
        yAxisLeft.axisMinimum = 0f // Giá trị thấp nhất của trục Y
        yAxisLeft.axisMaximum = 100f // Giá trị cao nhất của trục Y

        val yAxisRight = lineChart.axisRight
        yAxisRight.isEnabled = false // Vô hiệu hóa trục Y bên phải nếu không cần

        // Cấu hình nền của biểu đồ
        lineChart.setBackgroundColor(Color.BLACK) // Đặt nền biểu đồ thành màu đen
        lineChart.legend.textColor = Color.WHITE // Đổi màu chữ của chú thích

        // Thiết lập MarkerView hiển thị thông tin chi tiết khi chạm vào điểm
        val markerView = CustomMarkerView(requireContext(), R.layout.marker_view, fullTimestamps)
        lineChart.marker = markerView

        // Vẽ lại biểu đồ sau khi cập nhật
        lineChart.invalidate()
    }
}
