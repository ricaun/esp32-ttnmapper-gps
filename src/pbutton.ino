//----------------------------------------//
//  pbutton.ino
//
//  created 03/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

#define BUTTON 0
#define BUTTON_MODE_MAX 3

static int button_i;

void button_setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
}

boolean button_press()
{
  static boolean last;
  boolean now = digitalRead(BUTTON);
  boolean ret = (now == false & last == true);
  last = now;
  return ret;
}

boolean button_loop()
{
  if (button_press())
  {
    button_i++;
    return true;
  }
  return false;
}

int button_count()
{
  return button_i;
}

String button_mode()
{
  button_i = button_i % BUTTON_MODE_MAX;
  switch (button_i) {
    case 0:
      return "MODE UNCONFIRMED";
    case 1:
      return "MODE CONFIRMED";
    case 2:
      return "MODE SEND OFF";
  }
}
