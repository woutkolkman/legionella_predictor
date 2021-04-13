// is a test dummy temprature sensor 
float test_read_tempture(void) {
 static int i=0;
 const float values[] = {1,2,3,4,5,6};
  if(i >= 6) i = 0; 
 return values[i++];
}
