/*-------------------------------------------------
  Ascii string(len shorter than 8) to uint32_t number
	> more than 8 char return 0xFFFFFFFF;
	> any illegal char will return 0 (Can be edited.)
--------------------------------------------------
*/

static uint32_t ascii2num(uint8_t* str, uint32_t len)
{
  uint32_t i = 0;
	uint32_t result = 0;
  uint8_t tmpchar;
	
	if(len>8)
	{
    return 0xFFFFFFFF; /* overflow */
	}
	for(i=0; i<len; ++i)
	{
    result <<= 4; /* 1 char for 4 bit */
		tmpchar = str[i];
		if(tmpchar>='a' && tmpchar<='f')
		{
      result += tmpchar - 'a' + 10;		
		}
		else if(tmpchar>='A' && tmpchar<='F')
		{
      result += tmpchar - 'A' + 10;		
		}
		else if(tmpchar>='0' && tmpchar<='9')
		{
      result += tmpchar - '0';
		}
		else /* illegal char, return 0 */
		{
		  return 0; 
		}
	}
	return result;
}
