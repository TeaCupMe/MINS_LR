#ifndef PRICING_STRATEGY_H
#define PRICING_STRATEGY_H

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double calculate(double pricePerNight, long nights) const = 0;
};

class StandardPricing : public PricingStrategy {
public:
    double calculate(double pricePerNight, long nights) const override {
        return pricePerNight * nights;
    }
};

class LoyaltyDiscountPricing : public PricingStrategy {
public:
    double calculate(double pricePerNight, long nights) const override {
        return pricePerNight * nights * 0.9; // -10%
    }
};

class HolidayPricing : public PricingStrategy {
public:
    double calculate(double pricePerNight, long nights) const override {
        return pricePerNight * nights * 1.3; // +30%
    }
};

#endif