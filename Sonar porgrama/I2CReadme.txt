El código de I2c.h e I2c.cpp es válido solo con Windows 9x. En NT, 2000, Xp no funcionará debido a la proteción de I/O que poseen estos sistemas operativos.

I2c.h e I2c.cpp es código Visual C++ necesario para poder utilizar la comunicación I2C con la tarjeta interface Velleman K8000. 
 
I2c.h e I2c.cpp son versiones ligeramente modificadas de "Dino" para poder acceder a las funciones I2C de bajo nivel de la clase I2c directamente:

	void	GenerateStartCondition(void)
	void	GenerateAcknowledge(void)
	void	GenerateStopCondition(void)
	void	GenerateAcknowledgeFromMaster(void)
	void	OutputByteToI2C(int DataByte)
	int	InputByteFromI2C(void)
	void	UpdateIOArray(int Chip_no)

Para su utilización deberá:

a) Incluirse ambos ficheros en el directorio del workspace 
b) Incluir en el Header file del Cpp que lo utilice:

	#include "I2C.h"

c) Incluir en el Header file del Cpp que lo utilice, dentro de la definición de la clase que lo utilice, una derivación de clase:

	private:
		I2c	Board;

d) Incluir en el Cpp que lo utilice:

	#include "I2C.h"

e) Utilizar los diferentes métodos de la clase I2c con "Board." delante. Ej: Board.GenerateStopCondition(void)

Estas librerías para Visual C++ son equivalentes a las producidas por Velleman para Borlanc C. Para información sobre el uso de la librería y sus métodos, acceder a "I2CUsage.pdf"