void setup_twin_engine_collective()
{
  Wire.beginTransmission(15);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair.setZAxisRange(1023, 0);
    simchair.setThrottleRange(0,1023);//SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL, 1023);
    simchair.setRyAxisRange(0,1023);
    dev_twin_engine_collective = 1;
  }
}

void poll_twin_engine_collective()
{
  uint16_t z;
  uint16_t throttle;
  int16_t ry;

  Wire.requestFrom(15, 6);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read();
    byte b6 = Wire.read();

    z = b1;
    z = (z << 8) | b2;
    throttle = b3;
    throttle = (throttle << 8) | b4;
    ry = b5;
    ry = (ry << 8) | b6;
  }
  // uncomment the next line and turn your throttle to idle stop position to see SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL value 
  //Serial.println(throttle);
  if ((COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1) && (coll_head_mode_sw_position == 0))
  {
    if (COLL_HEAD_IDLE_STOP_COMPAT_PROFILE == "DCS_HUEY")
    {
      if (min_throttle_val[0] != TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL)
      {
         simchair.setThrottleRange(TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL, 1023);
         min_throttle_val[0] = TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL;
      }
     
      coll_head_idle_stop_compat_dcs (throttle,0,TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL,0);
      last_throttle_setting[0] = throttle;
      if (throttle < TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL)
      {
        throttle = TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL;
      }
      else
      {
        throttle_idle_cutoff[0] = 0; 
      }
      if ((idle_stop_pressed[0] != 1) && (throttle_idle_cutoff[0] == 0))
      {
        simchair.setThrottle(throttle);
      }
      else if (idle_stop_pressed[0] == 1)
      {
        throttle_idle_cutoff[0] = 1; 
      }   
    }
    else
    {
      //other profiles
      simchair.setThrottle(throttle);
      simchair.setRyAxis(ry);
    }
  }
  else
  {
    if (min_throttle_val[0] != 0)
    {
      simchair.setThrottleRange(0, 1023);
      simchair.setRyAxisRange(0,1023);
      min_throttle_val[0] = 0;
    }
    simchair.setThrottle(throttle);
    simchair.setRyAxis(ry);
  }
  simchair.setRyAxis(ry);
  
  if ((collective_hold_active == 0) && (collective_hold_position_set == 0))
  {
    simchair.setZAxis(z);
  }
  else if ((collective_hold_active == 1) && (collective_hold_position_set == 0))
  {
    collective_hold_position = z;
    collective_hold_position_set = 1;
  }
  else if ((collective_hold_active == 0) && (collective_hold_position_set == 1))
  {
    int one_percent_range = COLLECTIVE_ADC_RANGE / 100;
    int diff_z = z - collective_hold_position; // this is needed because of how the abs() works

    //if ((abs(diff_z)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
    if (abs(diff_z) < 2 * one_percent_range)
    {
      simchair.setZAxis(z);
      collective_hold_position_set = 0;
    }
  }
  
}
