#include "scales.h"
#include "pindef.h"

#if defined(SINGLE_HX711_CLOCK)
#include <HX711_2.h>
HX711_2 LoadCells;
#else
#include <HX711.h>
HX711 LoadCell_1; //HX711 1
HX711 LoadCell_2; //HX711 2
#endif

bool scalesPresent;

#ifdef SINGLE_BOARD
  unsigned char scale_clk = OUTPUT;
#else
  unsigned char scale_clk = OUTPUT_OPEN_DRAIN;
#endif

void scalesInit(float scalesF1, float scalesF2) {

  #if defined(SINGLE_HX711_CLOCK)
    LoadCells.begin(HX711_dout_1, HX711_dout_2, HX711_sck_1, HX711_sck_2, 128, scale_clk);
    LoadCells.set_scale(scalesF1, scalesF2);
    LoadCells.power_up();

    if (LoadCells.wait_ready_timeout(700, 50)) {
      LoadCells.tare(4);
      scalesPresent = true;
    }
  #else
    LoadCell_1.begin(HX711_dout_1, HX711_sck_1);
    LoadCell_2.begin(HX711_dout_2, HX711_sck_2);
    LoadCell_1.set_scale(scalesF1); // calibrated val1
    LoadCell_2.set_scale(scalesF2); // calibrated val2

    if (LoadCell_1.wait_ready_timeout(700, 100) && LoadCell_2.wait_ready_timeout(700, 100)) {
      scalesPresent = true;
      LoadCell_1.tare();
      LoadCell_2.tare();
    }
  #endif
}

void scalesTare(void) {
  #if defined(SINGLE_HX711_CLOCK)
    if (LoadCells.wait_ready_timeout(700, 100)) {
      TARE_AGAIN:
      LoadCells.tare(4);
      if (scalesGetWeight() < -0.1f && scalesGetWeight() > 0.1f) goto TARE_AGAIN;
    }
  #else
    if (LoadCell_1.wait_ready_timeout(700, 100) && LoadCell_2.wait_ready_timeout(700, 100)) {
      TARE_AGAIN:
      LoadCell_1.tare(2);
      LoadCell_2.tare(2);
      if (scalesGetWeight() < -0.1f && scalesGetWeight() > 0.1f) goto TARE_AGAIN;
    }

  #endif
}

float scalesGetWeight(void) {
  float currentWeight = 0;

  #if defined(SINGLE_HX711_CLOCK)
    if (LoadCells.wait_ready_timeout(200, 100)) {
      float values[2];
      LoadCells.get_units(values);
      currentWeight = values[0] + values[1];
    }
  #else
    if (LoadCell_1.wait_ready_timeout(200, 100) && LoadCell_2.wait_ready_timeout(200, 100)) {
      currentWeight = LoadCell_1.get_units() + LoadCell_2.get_units();
    }
  #endif

  return currentWeight;
}

bool scalesIsPresent(void) {
  return scalesPresent;
}
