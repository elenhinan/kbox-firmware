/*
  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <N2kMsg.h>
#include <N2kMessages.h>
#include "../KBoxTest.h"
#include "common/signalk/SKUnits.h"
#include "common/signalk/SKNMEA2000Parser.h"

TEST_CASE("SKNMEA2000Parser: Basic tests") {
  SKNMEA2000Parser p;
  tN2kMsg msg;

  SECTION("Parse invalid message") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 0 );
  }

  SECTION("128259: Speed in water") {
    SetN2kBoatSpeed(msg, 0, 3.4, N2kDoubleNA, N2kSWRT_Paddle_wheel);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 1 );
    CHECK( update.getNavigationSpeedThroughWater() == 3.4 );
  }

  SECTION("128259: Speed over ground") {
    SetN2kBoatSpeed(msg, 0, N2kDoubleNA, 3.4, N2kSWRT_Electro_magnetic);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 1 );
    CHECK( update.getNavigationSpeedOverGround() == 3.4 );
  }

  SECTION("128259: SOG and SOW together") {
    SetN2kBoatSpeed(msg, 0, 3.2, 3.4, N2kSWRT_Electro_magnetic);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 2 );
    CHECK( update.getNavigationSpeedThroughWater() == 3.2 );
    CHECK( update.getNavigationSpeedOverGround() == 3.4 );
  }

  SECTION("128267: Simple depth below transducer") {
    SetN2kWaterDepth(msg, 0, 42, N2kDoubleNA);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 1);
    CHECK( update.getEnvironmentDepthBelowTransducer() == 42 );
  }

  SECTION("128267: Depth below surface") {
    SetN2kWaterDepth(msg, 0, 42, 1.3);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 3);
    CHECK( update.getEnvironmentDepthBelowTransducer() == 42 );
    CHECK( update.getEnvironmentDepthBelowSurface() == 43.3 );
    CHECK( update.getEnvironmentDepthSurfaceToTransducer() == 1.3 );
  }

  SECTION("128267: Depth below keel") {
    SetN2kWaterDepth(msg, 0, 42, -1.2);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 3);
    CHECK( update.getEnvironmentDepthBelowTransducer() == 42 );
    CHECK( update.getEnvironmentDepthBelowKeel() == 40.8 );
    CHECK( update.getEnvironmentDepthTransducerToKeel() == 1.2 );
  }

  SECTION("127250: VESSEL True HEADING RAPID") {
    SetN2kTrueHeading(msg, 0, SKDegToRad(180));
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 1);
    CHECK( (update.getNavigationHeadingTrue() - SKDegToRad(180)) < 0.01 );
  }

  SECTION("127250: VESSEL Magnetic HEADING RAPID") {
    SetN2kMagneticHeading(msg, 0, 0.7898, -1.5883, 0.014); // 181°, -2°, 3.2°
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 2);
    CHECK( (update.getNavigationHeadingMagnetic() - 0.7898) < 0.01 );
    // CHECK( update.getNavigationMagneticDeviation() == -1.5883 );
    CHECK( update.getNavigationMagneticVariation() == 0.014 );
  }

  SECTION("127245: Rudder Angle") {
    SetN2kRudder(msg, SKDegToRad(-3.6), 0, N2kRDO_NoDirectionOrder, N2kDoubleNA);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 1);
    // Some precision is lost because NMEA2000 rounds this value to 0.0001 precision
    CHECK( update.getSteeringRudderAngle() == Approx(SKDegToRad(-3.6)).epsilon(0.0001) );
  }

  SECTION("130306: W I N D") {
    //TODO: watch if Timo will correct Typo
    SetN2kWindSpeed(msg, 0, 12.4, SKDegToRad(29.8), N2kWind_Apprent);
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    CHECK( update.getSize() == 2);
    CHECK( update.getEnvironmentWindSpeedApparent() == 12.4 );
    CHECK( update.getEnvironmentWindAngleApparent() == Approx(SKDegToRad(29.8)) );
  }
}
