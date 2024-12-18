package com.example.thingspeak.viewmodel

import android.util.Log
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import com.example.thingspeak.model.RetrofitInstance
import com.example.thingspeak.model.ThingSpeakData
import com.example.thingspeak.model.ThingSpeakResponse
import retrofit2.Call
import retrofit2.Callback
import retrofit2.Response

class ThingSpeakViewModel : ViewModel() {

    private val _data = MutableLiveData<List<ThingSpeakData>>()
    val data: LiveData<List<ThingSpeakData>> get() = _data

    fun fetchAllFields(apiKey: String, results: Int = 10) {
        // Sử dụng endpoint `feeds.json` để lấy tất cả các field
        val call = RetrofitInstance.api.getAllFieldData(apiKey, results)

        call.enqueue(object : Callback<ThingSpeakResponse> {
            override fun onResponse(
                call: Call<ThingSpeakResponse>,
                response: Response<ThingSpeakResponse>
            ) {
                if (response.isSuccessful) {
                    // Lấy danh sách dữ liệu từ API và lưu vào `_data`
                    _data.value = response.body()?.feeds ?: emptyList()
                } else {
                    Log.e("API_ERROR", "Response not successful: ${response.errorBody()?.string()}")
                }
            }

            override fun onFailure(call: Call<ThingSpeakResponse>, t: Throwable) {
                Log.e("API_ERROR", "Network error: ${t.message}")
            }
        })
    }
}
