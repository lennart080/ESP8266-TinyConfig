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
    NotInitialized,
    AlreadyInitialized,
    FileOpenFailed,
    FileWriteFailed,
    FileCreateFailed,    
    JsonParseFailed,
    FileSizeTooSmall,    
    FileSizeTooLarge,
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

    int getInt(const String& key, int fallback = 0);
    float getFloat(const String& key, float fallback = 0.0f);
    String getString(const String& key, const String& fallback = "");

    bool deleteKey(const String& key);

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