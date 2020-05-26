bool isValid(int gas_id, float gas_value) {
  if ( gas_id == 1 ) 
  {
    if ( gasNH3.value < 0 || ( gasNH3.value > 0 && gasNH3.value < gasNH3.minDetectable ) || gasNH3.value > gasNH3.maxDetectable )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 2 ) 
  {
    if ( gasCO.value < 0 || gasCO.value > 1000 )
    {
      return false;
    }
    else return true;
  }
  
  if ( gas_id == 3 ) 
  {
    if ( gasNO2.value < 0.5 || gasNO2.value > 10 )
    {
      return false;
    }
    else return true;
  }

  if ( gas_id == 4 ) 
  {
    if ( gasC3H8.value < 0 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 5 ) 
  {
    if ( gasC4H10.value < 0 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 6 ) 
  {
    if ( gasCH4.value < 0 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 7 ) 
  {
    if ( gasH2.value < 1 || gasH2.value > 1000 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 8 ) 
  {
    if ( gasC2H5OH.value < 10 || gasC2H5OH.value > 500 )
    {
      return false;
    }
    else return true;
  }
}
