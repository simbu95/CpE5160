// UART Generic Class

class UART {
public:
  UART(int Baud){
  
  }
  virtual void init();
  virtual void Send();
  virtual void Recieve();
private:
  int BaudRate;
};


class UART_LCD : public UART{
public:
  UART_LCD(int Baud){}
  void init();
  void Send();
  void Recieve();
private:
  int BaudRate;
}