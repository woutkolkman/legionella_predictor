// is a test dummy temprature sensor 
float test_read_tempture(void) {
 static int i=0;
 const float values[] = {24.0, 24.0, 23.0, 23.5,22.0,20.0,20.0,20.0,20.0};
  if(i >= 9) i = 0; 
 return values[i++];
}
