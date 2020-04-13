#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/spi/spi.h"

static const char *TAG = "dual_pot";

namespace esphome {
  namespace dual_pot {

class DualPotLight : public Component, public light::LightOutput, 
      public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
            spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_2MHZ> {

  public:
    void setup() override {
      ESP_LOGI(TAG, "DualPotLight setup started!");
      this->set_cs_pin(new GPIOPin(this->cs_pin_int_, OUTPUT, false)); //17);
      this->spi_setup();
      ESP_LOGI(TAG, "DualPotLight setup finished!");
      Component::setup();
    }
    void dump_config() override {
      Component::dump_config();
      LOG_PIN("  CS Pin: ", this->cs_);
    }
    //void set_switch_id(uint8_t switch_id) { this->switch_id_ = switch_id; }
    void set_min_value(uint32_t min_value) { min_value_ = min_value; }
    void set_max_value(uint32_t max_value) { max_value_ = max_value; }
    LightTraits get_traits() override {
      // return the traits this light supports
      auto traits = LightTraits();
      traits.set_supports_brightness(true);
      return traits;
    }

    void DigitalPotWrite(int cmd, int val)
    {
      // constrain input value within 0 - 255
      val = constrain(val, 0, 255);
      // set the CS pin to low to select the chip:
      digitalWrite(this->cs_pin_int_, LOW);
      // send the command and value via SPI:
      SPI.transfer(cmd);
      SPI.transfer(val);
      // Set the CS pin high to execute the command:
      digitalWrite(this->cs_pin_int_, HIGH);
    }
    //void setup_state(light::LightState *state) override;
    void write_state(LightState *state) override {
      //ESP_LOGI(TAG, "WRITE STATE WOOT: %.0f%%", this->state_->current_values.get_brightness());
      float b;
      state->current_values_as_brightness(&b);
      //state->current_values_as_brightness(&b);

      b=b*this->max_value_;

      this->DigitalPotWrite(0x11,b);

      ESP_LOGD(TAG, "  Brightness: %.0f%%", b);
      //ESP_LOGI(TAG, String("WRITE STATE WOOT") + dtostrf(16.111, 100, 2, charbuf));
    }

    void set_cs_pin_int(uint8_t pin) { this->cs_pin_int_ = pin; }
  
  protected:
  //void update_switch_(uint32_t value);
  //optional<uint8_t> switch_id_{};
  uint32_t min_value_ = 0;
  uint32_t max_value_ = 255;
  uint8_t cs_pin_int_ = 4;
  light::LightState *state_{nullptr};
 };
 
 } // namespace dual_pot
 } // namespace esphome
