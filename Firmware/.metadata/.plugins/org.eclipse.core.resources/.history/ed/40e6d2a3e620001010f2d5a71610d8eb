/*
    The host MCU (master) has to initiate the data transfers.
    The 7-bit device address of the slave depends on the state of
    the OTP I²C register 21 (0x15) bit 0… 4 + 2 I²C address selection
    pin 3 and 4.    

*/

#include "stdint.h"
#include "project.h"

//Direcciones de registros por macros
#define Address_FISICO 0x40 //Direccion de I2C del periferico

#define Pro_control 0x03
#define Address 0x15
#define OTP_Register_Zero_Position_h 0x16
#define OTP_Register_Zero_Position_l 0x17
#define Gain_control 0xFA
#define Diagnostico 0FB
#define Magnitud_h 0xFC
#define Magnitud_l 0xFD
#define ANGULO_HI 0xFE
#define ANGULO_LOW 0xFF


///BITFIELD DEL SENSOR SEGUN SU DATASHEET

typedef struct {
    
    // READOUT REGISTERS
    uint8_t angulo_Lo : 6;
    uint8_t angulo_Hi;
    uint8_t magnitud_Lo : 6;
    uint8_t magnitud_Hi;
    uint8_t diagnostico : 4;
    uint8_t Automatic_Gain_Control;

    // Programmable Customer Settings
    uint8_t OTP_Register_Zero_Position_Low_6_LSBs : 6;
    uint8_t OTP_Register_Zero_Position_Hi;
    uint8_t I2C_slave_address : 5;
        
    // Control OTP
    uint8_t Programming_Control : 7;
    
} AS5048B_registros;

// Definimos una estructura que agrupa el sensor y sus datos
struct AS5048B{
    AS5048B_registros registros;
};

//Funciones para hacer comunicacion con el sensor
void  AS5048B_Cero(struct AS5048B *registros); //Seteo del cero mecanico
void AS5048B_REGISTROS(struct AS5048B *registros); //Actualizamos las varibles de lectura de angulos

//Demás funciones para configurado del sensor ...