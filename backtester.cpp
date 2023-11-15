#include <iostream>
#include <fstream>
#include <string>
#include "orderbook.h"
#include <vector>

#define ADD_GAP 0.0005
#define MULTIPLY 1.3

double getPrice(std::ifstream& reader, bool printTime);
void onTick(double audPrice, double nzdPrice, OrderBook& orders);

OrderBook orders;
buffer n_gap;
std::string time;
double maxDrawdown = 100;
double total_lots = 0;

int main() {
    std::ifstream audPrices;
    std::ifstream nzdPrices;
    std::ofstream reporter;
    audPrices.open("AUDUSD60.csv");
    nzdPrices.open("NZDUSD60.csv");
    reporter.open("report.txt");
    reporter << "Deposited $10000!\n";
    orders.deposit(10000);
    while (!audPrices.eof() && !nzdPrices.eof()) {
        double audPrice = getPrice(audPrices, false);
        double nzdPrice = getPrice(nzdPrices, true);
        if (audPrice < nzdPrice) continue;
        if (audPrice == -1) break;
        reporter << "Time: " << time << std::endl;
        onTick(audPrice, nzdPrice, orders);
        auto h = orders.calculatePnL(audPrice - nzdPrice, reporter, true);
        double pnl = h.first;
        double lots = h.second;

        if (abs(pnl) > orders.balance * 0.8) {
            reporter << "Margin Call!\n";
            return 0;
        }
        
        if (pnl < 0) maxDrawdown = std::min(maxDrawdown, pnl / orders.balance * 100);
        orders.checkForClose(pnl, lots);
    }
    reporter << "Max drawdown is " << maxDrawdown << "%\n";
    reporter << "Total Lot is " << total_lots << std::endl;
    audPrices.close();
    nzdPrices.close();
    reporter.close();
    return 0;
}

double getPrice(std::ifstream& reader, bool printTime) {
    std::string priceDetails;
    reader >> priceDetails;
    int n = priceDetails.size();
    int n_comma = 0;
    if (printTime) {
        time = priceDetails.substr(0, 16);
    }
    for (int i = 0; i < n; ++ i) {
        if (priceDetails[i] == ',') {
            ++ n_comma;
            if (n_comma == 5) {
                return std::stod(priceDetails.substr(i + 1, 7));
            }
        }
    } 
    return -1;
}

void onTick(double audPrice, double nzdPrice, OrderBook& orders) {
    double priceGap = audPrice - nzdPrice;
    if (orders.hasOrder()) {
        const Order& lastOrder = orders.getLastOrder();
        if (lastOrder.dir == Action::LONG && 
            lastOrder.entryPrice - ADD_GAP >= priceGap) {
                orders.sendOrder(Action::LONG, priceGap, lastOrder.volume * MULTIPLY, total_lots);
            }
        else if (lastOrder.dir == Action::SHORT && 
            lastOrder.entryPrice + ADD_GAP <= priceGap) {
                orders.sendOrder(Action::SHORT, priceGap, lastOrder.volume * MULTIPLY, total_lots);
            }
        return;
    }
    n_gap.insert(priceGap);
    double MA = n_gap.getMA();
    
    
    if (priceGap < MA - 0.0005) {
        orders.sendOrder(Action::LONG, priceGap, 0.1, total_lots);
    } else if (priceGap > MA + 0.0005) {
        orders.sendOrder(Action::SHORT, priceGap, 0.1, total_lots);
    }
    return;
}
