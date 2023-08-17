El c�digo de I2c.h e I2c.cpp es v�lido solo con Windows 9x. En NT, 2000, Xp no funcionar� debido a la proteci�n de I/O que poseen estos sistemas operativos.

I2c.h e I2c.cpp es c�digo Visual C++ necesario para poder utilizar la comunicaci�n I2C con la tarjeta interface Velleman K8000. 
 
I2c.h e I2c.cpp son versiones ligeramente modificadas de "Dino" para poder acceder a las funciones I2C de bajo nivel de la clase I2c directamente:

	void	GenerateStartCondition(void)
	void	GenerateAcknowledge(void)
	void	GenerateStopCondition(void)
	void	GenerateAcknowledgeFromMaster(void)
	void	OutputByteToI2C(int DataByte)
	int	InputByteFromI2C(void)
	void	UpdateIOArray(int Chip_no)

Para su utilizaci�n deber�:

a) Incluirse ambos ficheros en el directorio del workspace 
b) Incluir en el Header file del Cpp que lo utilice:

	#include "I2C.h"

c) Incluir en el Header file del Cpp que lo utilice, dentro de la definici�n de la clase que lo utilice, una derivaci�n de clase:

	private:
		I2c	Board;

d) Incluir en el Cpp que lo utilice:

	#include "I2C.h"

e) Utilizar los diferentes m�todos de la clase I2c con "Board." delante. Ej: Board.GenerateStopCondition(void)

Estas librer�as para Visual C++ son equivalentes a las producidas por Velleman para Borlanc C. Para informaci�n sobre el uso de la librer�a y sus m�todos, acceder a "I2CUsage.pdf"