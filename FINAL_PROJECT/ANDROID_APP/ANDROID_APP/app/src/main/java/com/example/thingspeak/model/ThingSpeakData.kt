package com.example.thingspeak.model

import retrofit2.Call
import retrofit2.http.GET
import retrofit2.http.Query


/**
 * ThingSpeakData is data class corresponding with data fields received from API*/
data class ThingSpeakData(
    val field1: String?,
    val field2: String?,
    val field3: String?,
    val field4: String?,
    val created_at: String?
)

/**
 * ThingSpeakApi is interface where define HTTP request*/
interface ThingSpeakApi {
    // Endpoint để lấy dữ liệu từ ThingSpeak
    @GET("channels/2593173/feeds.json")
    fun getAllFieldData(
        @Query("api_key") apiKey: String,
        @Query("results") results: Int
    ): Call<ThingSpeakResponse>
}

/**
 *ThingSpeakResponse is data class including list of ThingSpeakData */
data class ThingSpeakResponse(val feeds: List<ThingSpeakData>)
