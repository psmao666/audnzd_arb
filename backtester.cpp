#include <iostream>
#include <fstream>
#include <string>
#include "orderbook.h"
#include <vector>

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
        double pnl = orders.calculatePnL(audPrice - nzdPrice, reporter, true);

        if (abs(pnl) > orders.balance * 0.8) {
            reporter << "Margin Call!\n";
            return 0;
        }
        
        if (pnl < 0) maxDrawdown = std::min(maxDrawdown, pnl / orders.balance * 100);
        orders.checkForClose(pnl);
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
            lastOrder.entryPrice - 0.001 >= priceGap) {
                orders.sendOrder(Action::LONG, priceGap, lastOrder.volume * 1.3, total_lots);
            }
        else if (lastOrder.dir == Action::SHORT && 
            lastOrder.entryPrice + 0.001 <= priceGap) {
                orders.sendOrder(Action::SHORT, priceGap, lastOrder.volume * 1.3, total_lots);
            }
        return;
    }
    n_gap.insert(priceGap);
    double MA = n_gap.getMA();
    
    
    if (priceGap < MA - 0.001) {
        orders.sendOrder(Action::LONG, priceGap, orders.balance / 500000, total_lots);
    } else if (priceGap > MA + 0.01) {
        orders.sendOrder(Action::SHORT, priceGap, orders.balance / 500000, total_lots);
    }
    orders.checkForClose(priceGap);
    return;
}
