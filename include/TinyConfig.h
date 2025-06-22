// Licensed under Apache License, Version 2.0
// SPDX-License-Identifier: Apache-2.0
// http://www.apache.org/licenses/LICENSE-2.0
// © 2025 Lennart Gutjahr

#pragma once
#include <LittleFS.h>
#include <ArduinoJson.h>

enum class TinyConfigError {
    None,
    FSInitFailed,
    FSNotRunning,
    FSAlreadyRunning,
    FileOpenFailed,
    FileWriteFailed,
    FileCreateFailed,    
    JsonParseFailed,
    JsonSerializeFailed,
    FileSizeTooSmall,    
    FileSizeTooLarge,
};

const std::unordered_map<TinyConfigError, String> TinyConfigErrorStrings = {
    {TinyConfigError::None, "No error"},
    {TinyConfigError::FSInitFailed, "Filesystem initialization failed"},
    {TinyConfigError::FSNotRunning, "TinyConfig not running"},
    {TinyConfigError::FSAlreadyRunning, "TinyConfig already running"},
    {TinyConfigError::FileOpenFailed, "Failed to open configuration file"},
    {TinyConfigError::FileWriteFailed, "Failed to write to configuration file"},
    {TinyConfigError::FileCreateFailed, "Failed to create configuration file"},
    {TinyConfigError::JsonParseFailed, "JSON parsing failed"},
    {TinyConfigError::JsonSerializeFailed, "JSON serialization failed"},
    {TinyConfigError::FileSizeTooSmall, "Configuration file size too small"},
    {TinyConfigError::FileSizeTooLarge, "Configuration file size too large"}
};

class TinyConfig {
public:
    bool StartTC();
    bool StopTC();
    bool resetConfig();
    bool setMaxFileSize(size_t maxSize);
    
    TinyConfigError getLastError() const;
    String getLastErrorString() const;

    bool set(const String& key, int value);
    bool set(const String& key, float value);
    bool set(const String& key, const String& value);

    bool deleteKey(const String& key);
    bool deleteKeys(const String keys[], size_t& count);
    bool deleteKeys(const std::vector<String>& keys);

    int getInt(const String& key, int fallback = 0);
    float getFloat(const String& key, float fallback = 0.0f);
    String getString(const String& key, const String& fallback = "");

    String getAll(const String& fallback = "{}");
    DynamicJsonDocument getAllJson();

private:
    TinyConfigError lastError = TinyConfigError::None;
    bool newFile();
    const char* FileString = "/config.json";
    bool isInitialized = false;
    size_t maxFileSize = 2048;

    bool loadDoc(ArduinoJson::DynamicJsonDocument& doc);
    bool saveDoc(const ArduinoJson::DynamicJsonDocument& doc);

    template <typename T>
    bool setInternal(const String& key, T value);
};