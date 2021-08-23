// SPI Generic Class

class SPI {
public:
  SPI(int Baud){
  
  }
  virtual void init();
  virtual void Send();
  virtual void Recieve();
private:
  int BaudRate;
};


class SPI_SD_Card : public UART{
public:
  SPI_SD_Card(int Baud){}
  void init();
  void Send();
  void Recieve();
private:
  int BaudRate;
}