// is a test dummy temprature sensor 
float test_read_tempture(void) {
 static int i=0;
 const float values[] = {24,24,23,22,20,20,20,21,21,21,24,24};
  if(i >= 12) i = 0; 
 return values[i++];
}
