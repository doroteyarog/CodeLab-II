#pragma once

#include "ofMain.h"
#include "json.hpp"
#include <map>
#include <string>
#include <vector>

using json = nlohmann::json;

struct CurrencyButton {
    std::string code;
    ofRectangle bounds;
};

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void keyPressed(int key) override;
    void mousePressed(int x, int y, int button) override;

    void urlResponse(ofHttpResponse& response);
    void fetchExchangeRates(const std::string& baseCurrency);
    void calculateConversion();
    bool validateNumericInput(const std::string& inputStr);
    void updateGridPositions();

    // App state
    std::string apiKey;
    std::string currentBaseCurrency;
    std::string currentTargetCurrency;
    std::string amountInputString;
    std::string conversionResultString;
    std::string statusMessage;

    bool isFetchingData;
    bool hasValidRates;

    // Supported Currencies & Page Scrolling
    std::vector<std::string> supportedCurrencies;
    std::vector<CurrencyButton> baseButtons;
    std::vector<CurrencyButton> targetButtons;
    std::map<std::string, double> exchangeRates;

    int baseScrollOffset;
    int targetScrollOffset;
    const int visibleRows = 4;
    const int cols = 4;
    const int itemsPerPage = 16;

    ofRectangle baseScrollUpBtn;
    ofRectangle baseScrollDownBtn;
    ofRectangle targetScrollUpBtn;
    ofRectangle targetScrollDownBtn;

    // Interactive Action Buttons
    ofRectangle convertButtonBounds;
    ofRectangle fetchButtonBounds;
    ofRectangle clearButtonBounds;
    ofRectangle backspaceButtonBounds;

    // Numpad Buttons
    struct KeypadButton {
        std::string label;
        ofRectangle bounds;
    };
    std::vector<KeypadButton> keypadButtons;
};