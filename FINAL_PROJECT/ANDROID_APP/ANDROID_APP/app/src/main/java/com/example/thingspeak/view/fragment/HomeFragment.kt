package com.example.thingspeak.view.fragment

import android.annotation.SuppressLint
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.annotation.RequiresApi
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import com.example.thingspeak.R
import com.example.thingspeak.model.ThingSpeakData
import com.example.thingspeak.viewmodel.ThingSpeakViewModel

class HomeFragment : Fragment() {

    private val viewModel: ThingSpeakViewModel by activityViewModels()
    private val handler = Handler(Looper.getMainLooper()) // Handler để chạy định kỳ
    private lateinit var updateRunnable: Runnable // Runnable cho việc tự động cập nhật

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_home, container, false)
    }

    @SuppressLint("SetTextI18n")
    @RequiresApi(Build.VERSION_CODES.O)
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val aqiIcon: ImageView = view.findViewById(R.id.aqiIcon) // Icon hình người đeo mặt nạ
        val aqiSubtitle: TextView = view.findViewById(R.id.aqiSubtitle)
        val aqiText: TextView = view.findViewById(R.id.aqiValue)
        val aqiLevelText: TextView = view.findViewById(R.id.aqiLevelText)
        val temperatureText: TextView = view.findViewById(R.id.valueTemperature)
        val humidityText: TextView = view.findViewById(R.id.valueHumidity)
        val coText: TextView = view.findViewById(R.id.valueCO)
        val dustText: TextView = view.findViewById(R.id.valueDust)

        // Observe data changed from ViewModel
        viewModel.data.observe(viewLifecycleOwner) { dataList ->
            val latestData = dataList.lastOrNull() // Dữ liệu mới nhất
            latestData?.let {
                temperatureText.text = "${it.field1?.toFloatOrNull() ?: 0f}°C"
                humidityText.text = "${it.field2?.toFloatOrNull() ?: 0f}%"
                coText.text = "${it.field3?.toFloatOrNull() ?: 0f} ppm"
                dustText.text = "${it.field4?.toFloatOrNull() ?: 0f} µg/m³"

                // Tính toán AQI và cập nhật giao diện
                val aqiLevel = calculateAQI(it)
                aqiIcon.setBackgroundResource(getAQIIconBackground(aqiLevel)) // Đổi hình nền icon
                aqiIcon.setColorFilter(getAQIIconColor(aqiLevel)) // Đổi màu icon

                // Get PM2.5 value
                aqiSubtitle.text =  "${it.field4?.toFloatOrNull() ?: 0f} µg/m³"
                aqiText.text = "${it.field4?.toFloatOrNull() ?: 0f}"
                aqiLevelText.text = aqiLevel
            }
        }

        // Cài đặt Runnable để tự động cập nhật dữ liệu mỗi 10 giây
        updateRunnable = Runnable {
            viewModel.fetchAllFields("YOUR_API_KEY", results = 100) // Gọi API để lấy dữ liệu mới
            handler.postDelayed(updateRunnable, 10000) // Tiếp tục cập nhật sau 10 giây
        }
        handler.post(updateRunnable) // Bắt đầu chu kỳ cập nhật tự động
    }

    override fun onDestroyView() {
        super.onDestroyView()
        handler.removeCallbacks(updateRunnable) // Xóa các callback để tránh rò rỉ bộ nhớ
    }

    private fun calculateAQI(data: ThingSpeakData): String {
        val aqi = (data.field4?.toFloatOrNull() ?: 0f)
        return when {
            aqi < 9 -> "Good"
            aqi < 35.4 -> "Moderate"
            aqi < 55.4 -> "Sensitive Unhealthy"
            aqi < 125.4 -> "Unhealthy"
            aqi < 225.4 -> "Very Unhealthy"
            else -> "Hazardous"
        }
    }

    private fun getAQIIconBackground(aqiLevel: String): Int {
        return when (aqiLevel) {
            "Good" -> R.drawable.circle_background_green
            "Moderate" -> R.drawable.circle_background_yellow
            "Sensitive Unhealthy" -> R.drawable.circle_background_orange
            "Unhealthy" -> R.drawable.circle_background_red
            "Very Unhealthy" -> R.drawable.circle_background_purple
            else -> R.drawable.circle_background_maroon
        }
    }

    private fun getAQIIconColor(aqiLevel: String): Int {
        return when (aqiLevel) {
            "Good" -> Color.WHITE
            else -> Color.BLACK
        }
    }
}
