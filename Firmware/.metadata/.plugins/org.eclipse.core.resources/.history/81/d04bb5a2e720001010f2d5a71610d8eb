#include "AS5048B.h"

//Funciones de protocolo I2C
static int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len){

    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
    uint16_t i = 0;

    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Stop       | -                   |
     * | Start      | -                   |
     * | Read       | (reg_data[0])       |
     * | Read       | (....)              |
     * | Read       | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */

    //psoc5
    rslt = I2C_MasterSendStart(dev_id,I2C_WRITE_XFER_MODE);//mANDA COMANDO
    rslt = I2C_MasterWriteByte(reg_addr);//Envia registro
    rslt = I2C_MasterSendRestart(dev_id,I2C_READ_XFER_MODE);//Paro para Leer datos
    
    while(i < (len - 1)) {
      
        reg_data[i] = I2C_MasterReadByte(I2C_ACK_DATA);//ack
        i++;
     
    }
    
    reg_data[i] = I2C_MasterReadByte(I2C_NAK_DATA);//nak
    rslt = I2C_MasterSendStop();
    
    return rslt;
}

static int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
    uint16_t i = 0;

    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Write      | (reg_data[0])       |
     * | Write      | (....)              |
     * | Write      | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */
    
    rslt = I2C_MasterSendStart(dev_id, I2C_WRITE_XFER_MODE);//rw =0  Escribimos comando
    rslt = I2C_MasterWriteByte(reg_addr);//Mandamos dirección de registo para escribir en el
    
    while(i < len) {
        
        rslt = I2C_MasterWriteByte(*reg_data);
        i++;
        reg_data++;
        
    }
    
    rslt = I2C_MasterSendStop();
    
    return rslt;
}

void AS5048B_REGISTROS(struct AS5048B *registros){
    
    uint8_t reg_data[10];
    
    user_i2c_read(Address_FISICO,Pro_control,reg_data,0);//Programming control
    user_i2c_read(Address_FISICO,Address,reg_data+1,2);//Registros 0x15 al 0x17  
    user_i2c_read(Address_FISICO,Gain_control,reg_data+4,5);//Registros 0xFA al 0xFF 
    
    //Pasamos todos lo consegido al bitfield
    registros->registros.Programming_Control = reg_data[0];
    registros->registros.I2C_slave_address = reg_data[1];
    registros->registros.OTP_Register_Zero_Position_Hi = reg_data[2];
    registros->registros.OTP_Register_Zero_Position_Low_6_LSBs = reg_data[3];
    registros->registros.Automatic_Gain_Control = reg_data[4];
    registros->registros.diagnostico = reg_data[5];
    registros->registros.magnitud_Hi = reg_data[6];
    registros->registros.magnitud_Lo = reg_data[7];
    registros->registros.angulo_Hi = reg_data[8];
    registros->registros.angulo_Lo= reg_data[9] >> 4;// sE ELIMINAN LOS 4 LBS como un filtrado
} 


void  AS5048B_Cero(struct AS5048B *registros){
    
    uint8_t reg_data[2] = {0,0};
    user_i2c_write(Address_FISICO,OTP_Register_Zero_Position_h,reg_data,1);//Escribimos 0 en OTP zero position registor
    
    AS5048B_REGISTROS(registros);//Actualizamos la lectura del estado actual de los registros
    
    reg_data[0] = registros->registros.angulo_Hi;
    reg_data[1] = registros->registros.angulo_Lo;
    user_i2c_write(Address_FISICO,OTP_Register_Zero_Position_h,reg_data,1);//Escribimos la ultima lectura de angulo al OTP zero register
    
}

