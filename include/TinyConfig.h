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
    Unknown,
    FileNotFound,
    NotInitialized,
    AlreadyInitialized,
    FileOpenFailed,
    FileWriteFailed,
    FileReadFailed,
    FileCerateFailed,
    JsonParseFailed,
    FileTooLarge
};

class TinyConfig {
public:
    bool StartTC();
    bool StopTC();
    bool resetConfig();
    void setMaxFileSize(size_t maxSize);
    
    TinyConfigError getLastError() const;
    String getLastErrorString() const;

    bool set(const String& key, int value);
    bool set(const String& key, float value);
    bool set(const String& key, const String& value);

    int getInt(const String& key, int fallback = 0);
    float getFloat(const String& key, float fallback = 0.0f);
    String getString(const String& key, const String& fallback = "");

private:
    TinyConfigError lastError = TinyConfigError::None;
    bool newFile();
    const char* FileString = "/config.json";
    bool isInitialized = false;
    size_t maxFileSize = 1024;

    bool loadDoc(ArduinoJson::DynamicJsonDocument& doc);
    bool saveDoc(const ArduinoJson::DynamicJsonDocument& doc);

    template <typename T>
    bool setInternal(const String& key, T value);
};