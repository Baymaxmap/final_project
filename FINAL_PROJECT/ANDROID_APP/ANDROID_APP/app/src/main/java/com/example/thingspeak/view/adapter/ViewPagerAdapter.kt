package com.example.thingspeak.view.adapter

import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import androidx.viewpager2.adapter.FragmentStateAdapter
import com.example.thingspeak.view.fragment.CoFragment
import com.example.thingspeak.view.fragment.DustFragment
import com.example.thingspeak.view.fragment.HomeFragment
import com.example.thingspeak.view.fragment.HumidityFragment
import com.example.thingspeak.view.fragment.TemperatureFragment

class ViewPagerAdapter(fragmentActivity: FragmentActivity) : FragmentStateAdapter(fragmentActivity) {

    // Số lượng trang
    override fun getItemCount(): Int = 5

    // Tạo Fragment tương ứng với từng vị trí (position)
    override fun createFragment(position: Int): Fragment {
        return when (position) {
            0 -> HomeFragment()
            1 -> TemperatureFragment()
            2 -> HumidityFragment()
            3 -> CoFragment()
            4 -> DustFragment()
            else -> Fragment() // Phòng trường hợp không đúng vị trí
        }
    }
}