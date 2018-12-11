/*
read extra barcode : boogie 
when it reads this:
preform dance + led actions for exactly 20 seconds, to do this we should use :

 unsigned long end_time = millis() + time_wanted;
  while(millis() < end_time){
  //do stuff
  }
  if(millis() >= end_time){
  //times up
  }
  
 to preform different effects all at the same time we can use combintations e.g 
 for(auto i = 0; i < 50; i++;){
	 .... go forward for i distance?
	 led effect changes as i changes...
 }
 
  
*/




