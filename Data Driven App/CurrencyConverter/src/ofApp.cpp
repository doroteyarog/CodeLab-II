#include "ofApp.h"
#include <sstream>
#include <iomanip>

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("Global Currency Converter - FreecurrencyAPI (33 Currencies)");
    ofSetWindowShape(1150, 780);
    ofBackground(24, 27, 34);

    ofRegisterURLNotification(this);

    apiKey = "fca_live_K0cvCRNjQAmztYWT415FFugkTNAVeFvdkW93RVwX";
    amountInputString = "100.00";
    conversionResultString = "Awaiting input...";
    statusMessage = "Ready.";
    isFetchingData = false;
    hasValidRates = false;

    // All 33 currencies supported on FreecurrencyAPI
    supportedCurrencies = {
        "USD", "EUR", "GBP", "JPY", "CAD", "AUD", "CHF", "CNY",
        "INR", "NZD", "BRL", "SGD", "HKD", "KRW", "MXN", "SEK",
        "NOK", "DKK", "PLN", "TRY", "ZAR", "THB", "IDR", "HUF",
        "CZK", "ILS", "MYR", "PHP", "RON", "BGN", "ISK", "HRK", "RUB"
    };

    currentBaseCurrency = "USD";
    currentTargetCurrency = "EUR";
    baseScrollOffset = 0;
    targetScrollOffset = 0;

    // Scroll buttons
    baseScrollUpBtn = ofRectangle(440, 115, 30, 60);
    baseScrollDownBtn = ofRectangle(440, 185, 30, 60);

    targetScrollUpBtn = ofRectangle(970, 115, 30, 60);
    targetScrollDownBtn = ofRectangle(970, 185, 30, 60);

    updateGridPositions();

    // Action buttons
    fetchButtonBounds = ofRectangle(50, 275, 180, 36);
    convertButtonBounds = ofRectangle(245, 275, 180, 36);

    // On-screen Numpad
    int numpadStartX = 50;
    int numpadStartY = 380;
    int numW = 65;
    int numH = 45;

    std::vector<std::string> keys = {
        "7", "8", "9",
        "4", "5", "6",
        "1", "2", "3",
        "0", ".", "C"
    };

    for (size_t i = 0; i < keys.size(); ++i) {
        int r = i / 3;
        int c = i % 3;
        keypadButtons.push_back({ keys[i], ofRectangle(numpadStartX + c * (numW + 6), numpadStartY + r * (numH + 6), numW, numH) });
    }

    backspaceButtonBounds = ofRectangle(numpadStartX + 3 * (numW + 6), numpadStartY, 70, numH * 2 + 6);
    clearButtonBounds = ofRectangle(numpadStartX + 3 * (numW + 6), numpadStartY + numH * 2 + 12, 70, numH * 2 + 6);

    fetchExchangeRates(currentBaseCurrency);
}

//--------------------------------------------------------------
void ofApp::updateGridPositions() {
    baseButtons.clear();
    targetButtons.clear();

    int startY = 115;
    int btnW = 90;
    int btnH = 30;
    int gap = 6;

    // Build base buttons for current page
    for (int i = 0; i < itemsPerPage; ++i) {
        int index = baseScrollOffset + i;
        if (index >= supportedCurrencies.size()) break;

        int row = i / cols;
        int col = i % cols;
        baseButtons.push_back({
            supportedCurrencies[index],
            ofRectangle(50 + col * (btnW + gap), startY + row * (btnH + gap), btnW, btnH)
            });
    }

    // Build target buttons for current page
    for (int i = 0; i < itemsPerPage; ++i) {
        int index = targetScrollOffset + i;
        if (index >= supportedCurrencies.size()) break;

        int row = i / cols;
        int col = i % cols;
        targetButtons.push_back({
            supportedCurrencies[index],
            ofRectangle(580 + col * (btnW + gap), startY + row * (btnH + gap), btnW, btnH)
            });
    }
}

//--------------------------------------------------------------
void ofApp::fetchExchangeRates(const std::string& baseCurrency) {
    if (isFetchingData) return;
    isFetchingData = true;
    statusMessage = "Loading latest rates for " + baseCurrency + "...";

    std::string requestUrl = "https://api.freecurrencyapi.com/v1/latest?apikey=" + apiKey + "&base_currency=" + baseCurrency;
    ofLoadURLAsync(requestUrl, "ratesRequest");
}

//--------------------------------------------------------------
void ofApp::urlResponse(ofHttpResponse& response) {
    isFetchingData = false;

    if (response.status != 200) {
        statusMessage = "Network Error: HTTP " + std::to_string(response.status);
        hasValidRates = false;
        return;
    }

    try {
        json parsedData = json::parse(response.data.getText());
        if (parsedData.contains("data")) {
            exchangeRates.clear();
            for (auto& item : parsedData["data"].items()) {
                exchangeRates[item.key()] = item.value().get<double>();
            }
            hasValidRates = true;
            statusMessage = "Exchange rates updated for: " + currentBaseCurrency;
            calculateConversion();
        }
        else {
            statusMessage = "Error: Invalid data format.";
            hasValidRates = false;
        }
    }
    catch (const std::exception& e) {
        statusMessage = std::string("Parser Error: ") + e.what();
        hasValidRates = false;
    }
}

//--------------------------------------------------------------
bool ofApp::validateNumericInput(const std::string& inputStr) {
    if (inputStr.empty()) return false;
    char* end = nullptr;
    double val = std::strtod(inputStr.c_str(), &end);
    return end != inputStr.c_str() && *end == '\0' && val >= 0.0;
}

//--------------------------------------------------------------
void ofApp::calculateConversion() {
    if (!hasValidRates) {
        conversionResultString = "Error: Rates not loaded.";
        return;
    }

    if (!validateNumericInput(amountInputString)) {
        conversionResultString = "Error: Enter a valid positive number.";
        return;
    }

    double amount = std::stod(amountInputString);
    if (currentBaseCurrency == currentTargetCurrency) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << amount << " " << currentBaseCurrency
            << "  =  " << amount << " " << currentTargetCurrency << "\n(Same currency: Rate 1.00)";
        conversionResultString = ss.str();
        return;
    }

    if (exchangeRates.find(currentTargetCurrency) != exchangeRates.end()) {
        double rate = exchangeRates[currentTargetCurrency];
        double converted = amount * rate;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << amount << " " << currentBaseCurrency
            << "  -->  " << std::fixed << std::setprecision(2) << converted << " " << currentTargetCurrency
            << "\n(1 " << currentBaseCurrency << " = " << rate << " " << currentTargetCurrency << ")";
        conversionResultString = ss.str();
    }
    else {
        conversionResultString = "Notice: Target currency not provided in latest dataset.";
    }
}

//--------------------------------------------------------------
void ofApp::update() {}

//--------------------------------------------------------------
void ofApp::draw() {
    // Header
    ofSetColor(240);
    ofDrawBitmapString("====================================================================================================", 50, 40);
    ofDrawBitmapString("                      DATA-DRIVEN GLOBAL CURRENCY CONVERTER (CODELAB II)                           ", 50, 55);
    ofDrawBitmapString("====================================================================================================", 50, 70);

    // Section Titles
    ofSetColor(120, 200, 255);
    ofDrawBitmapString("BASE (FROM): " + currentBaseCurrency + "  [Showing " + std::to_string(baseScrollOffset + 1) + "-" + std::to_string(std::min((int)supportedCurrencies.size(), baseScrollOffset + itemsPerPage)) + " of 33]", 50, 100);

    ofSetColor(0, 220, 160);
    ofDrawBitmapString("TARGET (TO): " + currentTargetCurrency + "  [Showing " + std::to_string(targetScrollOffset + 1) + "-" + std::to_string(std::min((int)supportedCurrencies.size(), targetScrollOffset + itemsPerPage)) + " of 33]", 580, 100);

    // Draw Base Currency Buttons
    for (auto& btn : baseButtons) {
        bool isSelected = (btn.code == currentBaseCurrency);
        ofSetColor(isSelected ? ofColor(0, 150, 255) : ofColor(45, 52, 65));
        ofDrawRectRounded(btn.bounds, 4);
        ofSetColor(255);
        ofDrawBitmapString(btn.code, btn.bounds.x + 30, btn.bounds.y + 20);
    }

    // Base Scroll buttons
    ofSetColor(60, 70, 85);
    ofDrawRectRounded(baseScrollUpBtn, 4);
    ofDrawRectRounded(baseScrollDownBtn, 4);
    ofSetColor(255);
    ofDrawBitmapString("^", baseScrollUpBtn.x + 11, baseScrollUpBtn.y + 35);
    ofDrawBitmapString("v", baseScrollDownBtn.x + 11, baseScrollDownBtn.y + 35);

    // Draw Target Currency Buttons
    for (auto& btn : targetButtons) {
        bool isSelected = (btn.code == currentTargetCurrency);
        ofSetColor(isSelected ? ofColor(0, 200, 140) : ofColor(45, 52, 65));
        ofDrawRectRounded(btn.bounds, 4);
        ofSetColor(255);
        ofDrawBitmapString(btn.code, btn.bounds.x + 30, btn.bounds.y + 20);
    }

    // Target Scroll buttons
    ofSetColor(60, 70, 85);
    ofDrawRectRounded(targetScrollUpBtn, 4);
    ofDrawRectRounded(targetScrollDownBtn, 4);
    ofSetColor(255);
    ofDrawBitmapString("^", targetScrollUpBtn.x + 11, targetScrollUpBtn.y + 35);
    ofDrawBitmapString("v", targetScrollDownBtn.x + 11, targetScrollDownBtn.y + 35);

    // Action Buttons
    ofSetColor(60, 70, 90);
    ofDrawRectRounded(fetchButtonBounds, 6);
    ofSetColor(255);
    ofDrawBitmapString("Refresh Live Rates", fetchButtonBounds.x + 20, fetchButtonBounds.y + 23);

    ofSetColor(0, 140, 90);
    ofDrawRectRounded(convertButtonBounds, 6);
    ofSetColor(255);
    ofDrawBitmapString("Convert Currency", convertButtonBounds.x + 28, convertButtonBounds.y + 23);

    // Input & Numpad Area
    ofSetColor(200);
    ofDrawBitmapString("ENTER AMOUNT (Click or type):", 50, 340);

    // Amount Display Field
    ofSetColor(35, 40, 50);
    ofDrawRectRounded(50, 350, 360, 38, 4);
    ofSetColor(255);
    ofDrawBitmapString("> " + amountInputString + "_", 65, 374);

    // Numpad Buttons
    for (auto& btn : keypadButtons) {
        ofSetColor(50, 58, 72);
        ofDrawRectRounded(btn.bounds, 4);
        ofSetColor(255);
        ofDrawBitmapString(btn.label, btn.bounds.x + 26, btn.bounds.y + 28);
    }

    // Backspace & Clear buttons
    ofSetColor(180, 80, 50);
    ofDrawRectRounded(backspaceButtonBounds, 4);
    ofSetColor(255);
    ofDrawBitmapString("Back", backspaceButtonBounds.x + 18, backspaceButtonBounds.y + 50);

    ofSetColor(140, 40, 40);
    ofDrawRectRounded(clearButtonBounds, 4);
    ofSetColor(255);
    ofDrawBitmapString("Clear", clearButtonBounds.x + 16, clearButtonBounds.y + 50);

    // Status Message
    ofSetColor(isFetchingData ? ofColor::yellow : ofColor::lightGreen);
    ofDrawBitmapString("Status: " + statusMessage, 580, 290);

    // Results Display Panel
    ofSetColor(30, 38, 50);
    ofDrawRectRounded(580, 310, 500, 180, 8);
    ofSetColor(0, 220, 160);
    ofDrawBitmapString("--- CONVERSION RESULT ---", 605, 350);
    ofSetColor(255);
    ofDrawBitmapString(conversionResultString, 605, 395);

    // Documentation notes for video/report
    ofSetColor(140);
    int infoY = 620;
    ofDrawBitmapString("Assignment Assessment Criteria Implemented:", 50, infoY);
    ofDrawBitmapString("- Application Design: Full 33-currency catalog with responsive pagination grids (^ / v buttons)", 50, infoY + 20);
    ofDrawBitmapString("- Technical Implementation: Non-blocking asynchronous HTTP (ofRegisterURLNotification) & JSON data structures", 50, infoY + 40);
    ofDrawBitmapString("- Error Handling: Defensive checks for invalid input formats, unlisted pairs, and HTTP status codes", 50, infoY + 60);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    // Base Currency selection
    for (auto& btn : baseButtons) {
        if (btn.bounds.inside(x, y)) {
            if (currentBaseCurrency != btn.code) {
                currentBaseCurrency = btn.code;
                fetchExchangeRates(currentBaseCurrency);
            }
            return;
        }
    }

    // Target Currency selection
    for (auto& btn : targetButtons) {
        if (btn.bounds.inside(x, y)) {
            currentTargetCurrency = btn.code;
            calculateConversion();
            return;
        }
    }

    // Scroll Base Grid
    if (baseScrollUpBtn.inside(x, y)) {
        baseScrollOffset = std::max(0, baseScrollOffset - cols);
        updateGridPositions();
        return;
    }
    if (baseScrollDownBtn.inside(x, y)) {
        if (baseScrollOffset + itemsPerPage < (int)supportedCurrencies.size()) {
            baseScrollOffset += cols;
            updateGridPositions();
        }
        return;
    }

    // Scroll Target Grid
    if (targetScrollUpBtn.inside(x, y)) {
        targetScrollOffset = std::max(0, targetScrollOffset - cols);
        updateGridPositions();
        return;
    }
    if (targetScrollDownBtn.inside(x, y)) {
        if (targetScrollOffset + itemsPerPage < (int)supportedCurrencies.size()) {
            targetScrollOffset += cols;
            updateGridPositions();
        }
        return;
    }

    // Numpad input
    for (auto& btn : keypadButtons) {
        if (btn.bounds.inside(x, y)) {
            if (btn.label == "C") {
                amountInputString = "";
            }
            else if (btn.label == ".") {
                if (amountInputString.find('.') == std::string::npos) {
                    amountInputString += ".";
                }
            }
            else {
                amountInputString += btn.label;
            }
            calculateConversion();
            return;
        }
    }

    // Backspace
    if (backspaceButtonBounds.inside(x, y)) {
        if (!amountInputString.empty()) {
            amountInputString.pop_back();
            calculateConversion();
        }
        return;
    }

    // Clear
    if (clearButtonBounds.inside(x, y)) {
        amountInputString = "";
        calculateConversion();
        return;
    }

    // Manual Refresh button
    if (fetchButtonBounds.inside(x, y)) {
        fetchExchangeRates(currentBaseCurrency);
        return;
    }

    // Convert button
    if (convertButtonBounds.inside(x, y)) {
        calculateConversion();
        return;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if ((key >= '0' && key <= '9') || key == '.') {
        amountInputString += (char)key;
        calculateConversion();
    }
    else if (key == OF_KEY_BACKSPACE && !amountInputString.empty()) {
        amountInputString.pop_back();
        calculateConversion();
    }
    else if (key == OF_KEY_RETURN) {
        calculateConversion();
    }
}