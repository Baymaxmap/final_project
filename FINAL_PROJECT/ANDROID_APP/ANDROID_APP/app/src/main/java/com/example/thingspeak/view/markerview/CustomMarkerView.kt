package com.example.thingspeak.view.markerview

import android.annotation.SuppressLint
import android.content.Context
import android.widget.TextView
import com.example.thingspeak.R
import com.github.mikephil.charting.components.MarkerView
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.highlight.Highlight
import com.github.mikephil.charting.utils.MPPointF

@SuppressLint("ViewConstructor")
class CustomMarkerView(
    context: Context,
    layoutResource: Int,
    private val timestamps: List<String>
) : MarkerView(context, layoutResource) {

    private val tvContent: TextView = findViewById(R.id.tvContent)

    override fun refreshContent(e: Entry, highlight: Highlight?) {
        val index = e.x.toInt()
        val timestamp = timestamps.getOrNull(index) ?: "Unknown"

        // Tách ngày và giờ từ timestamp
        val date = timestamp.substringBefore(" ")
        val time = timestamp.substringAfter(" ")

        // Hiển thị ngày và giờ trên hai dòng
        tvContent.text = "Date: $date\nTime: $time\nValue: ${e.y}"
        super.refreshContent(e, highlight)
    }


    override fun getOffset(): MPPointF {
        return MPPointF(-(width / 2).toFloat(), -height.toFloat())
    }
}
