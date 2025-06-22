// Licensed under Apache License, Version 2.0
// SPDX-License-Identifier: Apache-2.0
// http://www.apache.org/licenses/LICENSE-2.0
// © 2025 Lennart Gutjahr

#include "TinyConfig.h"
using namespace ArduinoJson;

/**
 * @brief Initializes the TinyConfig system and filesystem.
 * @return true if initialization succeeded, false otherwise.
 * 
 * This function mounts the LittleFS filesystem and checks if the configuration file exists.
 * If the file does not exist, it attempts to create a new configuration file with an empty JSON object.
 * If the filesystem is already initialized, check getLastError() or getLastErrorString() for details.
 * If the filesystem cannot be mounted, check getLastError() or getLastErrorString() for details.
 */
bool TinyConfig::StartTC() {
    if (isInitialized) {
        lastError = TinyConfigError::FSAlreadyRunning;
        return false;
    }
    if (!LittleFS.begin()) {
        lastError = TinyConfigError::FSInitFailed;
        return false;
    }
    if (!LittleFS.exists(FileString)) {
        if (!resetConfig()) {
            lastError = TinyConfigError::FileCreateFailed;
            return false;
        }
    }
    lastError = TinyConfigError::None;
    isInitialized = true;
    return true;
}

/**
 * @brief Stops the TinyConfig system and unmounts the filesystem.
 * @return true if stopped successfully, false otherwise.
 * 
 * This function unmounts the LittleFS filesystem and sets the initialized flag to false.
 * If the system is not initialized. On failure, check getLastError() or getLastErrorString() for details.
 */
bool TinyConfig::StopTC() {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return false;
    }
    LittleFS.end();
    isInitialized = false;
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Resets the configuration file to an empty JSON object.
 * @return true if reset succeeded, false otherwise.
 * 
 * This function opens the configuration file in write mode and clears its contents.
 * If the file cannot be created or opened, check getLastError() or getLastErrorString() for details.
 * If the filesystem is not initialized. On failure, check getLastError() or getLastErrorString() for details.
 */
bool TinyConfig::resetConfig() {
    File file = LittleFS.open(FileString, "w");
    if (!file) {
        lastError = TinyConfigError::FileCreateFailed;
        return false;
    }
    file.print("{}");
    file.close();
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Gets the last error code.
 * @return The last TinyConfigError value. This enum indicates the reason for the last failure, or None if no error.
 */
TinyConfigError TinyConfig::getLastError() const {
    return lastError;
}

/**
 * @brief Gets the last error as a string.
 * @return String representation of the last error code. Useful for debugging or logging.
 */
String TinyConfig::getLastErrorString() const {
    auto it = TinyConfigErrorStrings.find(lastError);
    return (it != TinyConfigErrorStrings.end()) ? it->second : "unknown error";
}

/**
 * @brief Sets the maximum allowed file size for the configuration file.
 * @param maxSize The maximum file size in bytes. If the config exceeds this size, set operations will fail.
 * @return true if the size was set successfully, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 * 
 * This function sets the maximum file size for the configuration file.
 * If the provided size is less than 9 bytes or greater than 4096 bytes, it sets the lastError to FileSizeToSmall or FileSizeTooLarge respectively.
 * If the size is valid, it updates maxFileSize and sets lastError to None.
 * Changing the maximum file size affects all subsequent set operations. It does not change the size of the existing file.
 */
bool TinyConfig::setMaxFileSize(size_t maxSize) {
    if (maxSize < 9) {
        lastError = TinyConfigError::FileSizeTooSmall;
        return false;
    }
    if (maxSize > 4096) {
        lastError = TinyConfigError::FileSizeTooLarge;
        return false;
    }
    maxFileSize = maxSize;
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Loads the configuration file into a DynamicJsonDocument.
 * @param doc Reference to the DynamicJsonDocument to load into.
 * @return true if loading succeeded, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 * 
 * This function opens the configuration file in read mode and attempts to deserialize its contents into the provided DynamicJsonDocument.
 * If the file cannot be opened or read, or if the JSON parsing fails, it sets the lastError accordingly.
 * If the filesystem is not initialized, it sets the lastError to FSNotRunning.
 * If the file is successfully loaded, it sets lastError to None.
 */
bool TinyConfig::loadDoc(DynamicJsonDocument& doc) {
    File f = LittleFS.open(FileString, "r");
    if (!f) {
        lastError = TinyConfigError::FileOpenFailed;
        return false;
    }
    auto err = deserializeJson(doc, f);
    f.close();
    if (err) {
        lastError = TinyConfigError::JsonParseFailed;
        return false;
    }
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Saves a DynamicJsonDocument to the configuration file.
 * @param doc The DynamicJsonDocument to save.
 * @return true if saving succeeded, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 * 
 * This function opens the configuration file in write mode and serializes the provided DynamicJsonDocument to it.
 * If the file cannot be opened or written to, it sets the lastError accordingly.
 */
bool TinyConfig::saveDoc(const DynamicJsonDocument& doc) {
    File f = LittleFS.open(FileString, "w");
    if (!f) {
        lastError = TinyConfigError::FileOpenFailed;
        return false;
    }
    if (serializeJson(doc, f) == 0) {
        lastError = TinyConfigError::FileWriteFailed;
        f.close();
        return false;
    }
    f.close();
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Internal helper to set a value in the configuration.
 * @tparam T The type of the value to set.
 * @param key The key to set.
 * @param value The value to set.
 * @return true if the value was set successfully, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 * 
 * This function loads the configuration file into a DynamicJsonDocument, sets the specified key to the provided value,
 * and saves the document back to the file. It checks if the file size exceeds the maximum allowed size.
 * If the filesystem is not initialized, it sets the lastError to FSNotRunning.
 * If the file size exceeds maxFileSize, it sets the lastError to FileTooLarge.
 * If the file is successfully updated, it sets lastError to None.
 */
template <typename T>
bool TinyConfig::setInternal(const String& key, T value) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return false;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return false;
    }
    doc[key] = value;
    String jsonString;
    serializeJson(doc, jsonString);
    if (jsonString.length() > maxFileSize) {
        lastError = TinyConfigError::FileSizeTooLarge;
        return false;
    }
    if (!saveDoc(doc)) {
        return false;
    }
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Sets or updates an integer value in the configuration.
 * @param key The key to set.
 * @param value The integer value to set.
 * @return true if the value was set successfully, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 */
bool TinyConfig::set(const String& key, int value) {
    return setInternal(key, value);
}

/**
 * @brief Sets or updates a float value in the configuration.
 * @param key The key to set.
 * @param value The float value to set.
 * @return true if the value was set successfully, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 */
bool TinyConfig::set(const String& key, float value) {
    return setInternal(key, value);
}

/**
 * @brief Sets or updates a string value in the configuration.
 * @param key The key to set.
 * @param value The string value to set.
 * @return true if the value was set successfully, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 */
bool TinyConfig::set(const String& key, const String& value) {
    return setInternal(key, value);
}

/**
 * @brief Gets an integer value from the configuration.
 * @param key The key to retrieve.
 * @param fallback The fallback value if the key does not exist or on error.
 * @return The integer value or fallback. If the key does not exist or an error occurs, the fallback value is returned.
 * 
 * Check getLastError() or getLastErrorString() for details on any errors that occur.
 */
int TinyConfig::getInt(const String& key, int fallback) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return fallback;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return fallback;
    }
    lastError = TinyConfigError::None;
    return doc[key] | fallback;
}

/**
 * @brief Gets a float value from the configuration.
 * @param key The key to retrieve.
 * @param fallback The fallback value if the key does not exist or on error.
 * @return The float value or fallback. If the key does not exist or an error occurs, the fallback value is returned.
 * 
 * Check getLastError() or getLastErrorString() for details on any errors that occur.
 */
float TinyConfig::getFloat(const String& key, float fallback) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return fallback;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return fallback;
    }
    lastError = TinyConfigError::None;
    return doc[key] | fallback;
}

/**
 * @brief Gets a string value from the configuration. 
 * @param key The key to retrieve.
 * @param fallback The fallback value if the key does not exist or on error.
 * @return The string value or fallback. If the key does not exist or an error occurs, the fallback value is returned.
 * 
 * Check getLastError() or getLastErrorString() for details on any errors that occur.
 */
String TinyConfig::getString(const String& key, const String& fallback) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return fallback;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return fallback;
    }
    lastError = TinyConfigError::None;
    return doc[key] | fallback;
}

/**
 * @brief Gets all configuration data as a DynamicJsonDocument.
 * @return A DynamicJsonDocument representing the entire configuration.
 * 
 * This function loads the entire configuration into a DynamicJsonDocument.
 * If the filesystem is not initialized, it sets the lastError to FSNotRunning.
 * If the file cannot be loaded, it sets lastError accordingly.
 */
DynamicJsonDocument TinyConfig::getAllJson() {
    DynamicJsonDocument doc(maxFileSize);
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return doc;
    }
    if (!loadDoc(doc)) {
        return doc;
    }
    lastError = TinyConfigError::None;
    return doc;
}

/**
 * @brief Gets all configuration data as a JSON string.
 * @param fallback The fallback value if the configuration is empty or an error occurs.
 * @return A JSON string representing the entire configuration or the fallback value.
 * 
 * This function loads the entire configuration into a DynamicJsonDocument and serializes it to a string.
 * If the filesystem is not initialized, it sets the lastError to FSNotRunning.
 * If the file cannot be loaded, it returns the fallback value and sets lastError accordingly.
 */
String TinyConfig::getAll(const String& fallback) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return fallback;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return fallback;
    }
    String jsonString;
    if (serializeJson(doc, jsonString) == 0) {
        lastError = TinyConfigError::JsonSerializeFailed;
        return fallback;
    }
    lastError = TinyConfigError::None;
    return jsonString;
}

/**
 * @brief Deletes a key + data from the configuration.
 * @param key The key to delete.
 * @return true if the key was deleted successfully, false otherwise. On failure, check getLastError() or getLastErrorString() for details.
 * 
 * This funktion removes the specified key from the configuration file.
 * If the filesystem is not initialized, it sets the lastError to FSNotRunning.
 * If the key does not exist, it sets lastError to None and returns false.
 * If the file is successfully updated, it sets lastError to None.
 */
bool TinyConfig::deleteKey(const String& key) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return false;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return false;
    }
    if (!doc.containsKey(key)) {
        lastError = TinyConfigError::None;
        return false;
    }
    doc.remove(key);
    if (!saveDoc(doc)) {
        return false;
    }
    lastError = TinyConfigError::None;
    return true;
}

/**
 * @brief Deletes multiple keys from the configuration.
 * @param keys Array of keys to delete.
 * @param count Number of keys in the array.
 * @return true if at least one key was deleted, false otherwise.
 */
bool TinyConfig::deleteKeys(const String keys[], size_t& count) {
    std::vector<String> keyVector(keys, keys + count);
    return deleteKeys(keyVector);
}

/**
 * @brief Deletes multiple keys from the configuration.
 * @param keys Vector of keys to delete.
 * @return true if at least one key was deleted, false otherwise.
 */
bool TinyConfig::deleteKeys(const std::vector<String>& keys) {
    if (!isInitialized) {
        lastError = TinyConfigError::FSNotRunning;
        return false;
    }
    DynamicJsonDocument doc(maxFileSize);
    if (!loadDoc(doc)) {
        return false;
    }
    bool deleted = false;
    for (const auto& key : keys) {
        if (doc.containsKey(key)) {
            doc.remove(key);
            deleted = true;
        }
    }
    if (deleted) {
        if (!saveDoc(doc)) {
            return false;
        }
    }
    lastError = TinyConfigError::None;
    return deleted;
}

// Explicit template instantiations
template bool TinyConfig::setInternal<int>(const String&, int);
template bool TinyConfig::setInternal<float>(const String&, float);
template bool TinyConfig::setInternal<String>(const String&, String);
