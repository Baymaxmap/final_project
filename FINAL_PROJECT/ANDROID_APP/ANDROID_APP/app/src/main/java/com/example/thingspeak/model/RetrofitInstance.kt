package com.example.thingspeak.model

import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory

/**
 * 1. RetrofitInstance is used to return Retrofit object
 *
 * 2. Retrofit is used to config method of SENDING HTTP request and RECEIVING data returned
 * for example, code below: .baseUrl() used for sending HTTP request, .addConverter...used for receiving data
 *
 * 3. For really sending request and receiving data, must create an object having endpoint of Url
 * and define the return data type of these request -> ThingSpeakApi object*/
object RetrofitInstance {
    private const val BASE_URL = "https://api.thingspeak.com/"

    val api: ThingSpeakApi by lazy {
        Retrofit.Builder()      //create a builder to configure
            .baseUrl(BASE_URL)  //endpoint API will be concatenated with this URL to impl HTTP requests
            .addConverterFactory(GsonConverterFactory.create()) //let to use Gson to convert JSON into Kotlin objects
            .build()    //create Retrofit object
            .create(ThingSpeakApi::class.java)  //create implementation of interface ThingSpeakApi
    }
}