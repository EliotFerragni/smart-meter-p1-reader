# Simple Smart Meter P1 reader over ZigBee

Here is a simple code to read HDLC/DLSM/COSEM frames coming from an Iskra AM550 from Romande Energie, one of the biggest electricity provider in [Romandy, Switzerland](https://en.wikipedia.org/wiki/Romandy) and publish it to zigbee. This project uses a simple [esp32-h2 devkit board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/esp32-h2-devkitm-1/user_guide.html#getting-started) and an RJ12 receptacle soldered to its pins. That's all. No need for something else except an RJ12 cable.

When reaching Romande Energie to ask the specs, if the P1 port should be activated and if the link is encrypted, they sent me [this document](docs/Courrier_CII_Annexe_Iskra_PROD.pdf) and told me the link is enabled by default and not encrypted. After asking them if it was possible to switch to a more simple protocol (there is one which outputs ASCII), they responded if was not possible.

## Protocol
Here is a dump of what is sent every **5 seconds** by the smart meter I have (manufacturing numbers are fake for obvious reasons). The total length is **380 bytes**. Since I changed the data to hide the manufacturing numbers, the HDLC frames are no longer valid for CRC reasons.

7EA8A4CF0223039996E6E7000F000002F10C07E808130114210A00FF88800210020209060000600100FF0908FFFFFFFFFFFFFFFFF020209060000600101FF0907FFFFFFFFFFFFFFFFF020309060100010700FF060000000002020F00161B020309060100020700FF060000004302020F00161B020309060100010800FF06000209DA02020F00161E020309060100020800FF06000028C402020F00161E02030906010020072FE07E7EA8A4CF022303999600FF12092C02020FFF1623020309060100340700FF12092302020FFF1623020309060100480700FF12093402020FFF16230203090601001F0700FF12000002020FFE1621020309060100330700FF12001202020FFE1621020309060100470700FF12004602020FFE1621020309060100010801FF060000F54E02020F00161E020309060100010802FF060001148C02020F00161E020309060100845B7E7EA02ECF022313BD61020801FF06000019B902020F00161E020309060100020802FF0600000F0B02020F00161E649A7E

When pasting the original data in the [Gurux DLMS Translator](https://www.gurux.fi/GuruxDLMSTranslator), we get the following result:

1. **Translate** button  
We can see the smart metter sends 3 HDLC frames.
``` xml
<HDLC len="163" >
<TargetAddress Value="103" />
<SourceAddress Value="145" />
<!--S frame.-->
<FrameType Value="03" />
<NextFrame Value="E6E7000F000002F10C07E808130114210A00FF88800210020209060000600100FF0908FFFFFFFFFFFFFFFFF020209060000600101FF0907FFFFFFFFFFFFFFF020309060100010700FF060000000002020F00161B020309060100020700FF060000004302020F00161B020309060100010800FF06000209DA02020F00161E020309060100020800FF06000028C402020F00161E0203090601002007" />
</HDLC>
<HDLC len="163" >
<TargetAddress Value="103" />
<SourceAddress Value="145" />
<!--S frame.-->
<FrameType Value="03" />
<NextFrame Value="00FF12092C02020FFF1623020309060100340700FF12092302020FFF1623020309060100480700FF12093402020FFF16230203090601001F0700FF12000002020FFE1621020309060100330700FF12001202020FFE1621020309060100470700FF12004602020FFE1621020309060100010801FF060000F54E02020F00161E020309060100010802FF060001148C02020F00161E020309060100" />
</HDLC>
<HDLC len="45" >
<TargetAddress Value="103" />
<SourceAddress Value="145" />
<!--S frame.-->
<FrameType Value="13" />
<PDU>
<Data="0801FF06000019B902020F00161E020309060100020802FF0600000F0B02020F00161E" />
</PDU>
</HDLC>
```
2. **To Conformance Test** button  
And here the parsed data. 
```xml
<?xml version="1.0" encoding="utf-8"?>
<Messages><GatewayRequest>
  <NetworkId Value="231"/>
  <PhysicalDeviceAddress Value=""/>
  <DataNotification>
    <LongInvokeIdAndPriority Value="753"/>
    <!--2024-08-19 20:33:10-->
    <DateTime Value="07E808130114210A00FF8880"/>
    <NotificationBody>
      <DataValue>
        <Structure Qty="16">
          <Structure Qty="2">
            <!--0.0.96.1.0.255-->
            <OctetString Value="0000600100FF"/>
            <!--85745614-->
            <OctetString Value="FFFFFFFFFFFFFFFFF"/>
          </Structure>
          <Structure Qty="2">
            <!--0.0.96.1.1.255-->
            <OctetString Value="0000600101FF"/>
            <!--1065863-->
            <OctetString Value="FFFFFFFFFFFFFFF"/>
          </Structure>
          <Structure Qty="3">
            <!--1.0.1.7.0.255-->
            <OctetString Value="0100010700FF"/>
            <UInt32 Value="0"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="27"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.2.7.0.255-->
            <OctetString Value="0100020700FF"/>
            <UInt32 Value="67"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="27"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.1.8.0.255-->
            <OctetString Value="0100010800FF"/>
            <UInt32 Value="133594"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="30"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.2.8.0.255-->
            <OctetString Value="0100020800FF"/>
            <UInt32 Value="10436"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="30"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.32.7.0.255-->
            <OctetString Value="0100200700FF"/>
            <UInt16 Value="2348"/>
            <Structure Qty="2">
              <Int8 Value="-1"/>
              <Enum Value="35"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.52.7.0.255-->
            <OctetString Value="0100340700FF"/>
            <UInt16 Value="2339"/>
            <Structure Qty="2">
              <Int8 Value="-1"/>
              <Enum Value="35"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.72.7.0.255-->
            <OctetString Value="0100480700FF"/>
            <UInt16 Value="2356"/>
            <Structure Qty="2">
              <Int8 Value="-1"/>
              <Enum Value="35"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.31.7.0.255-->
            <OctetString Value="01001F0700FF"/>
            <UInt16 Value="0"/>
            <Structure Qty="2">
              <Int8 Value="-2"/>
              <Enum Value="33"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.51.7.0.255-->
            <OctetString Value="0100330700FF"/>
            <UInt16 Value="18"/>
            <Structure Qty="2">
              <Int8 Value="-2"/>
              <Enum Value="33"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.71.7.0.255-->
            <OctetString Value="0100470700FF"/>
            <UInt16 Value="70"/>
            <Structure Qty="2">
              <Int8 Value="-2"/>
              <Enum Value="33"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.1.8.1.255-->
            <OctetString Value="0100010801FF"/>
            <UInt32 Value="62798"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="30"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.1.8.2.255-->
            <OctetString Value="0100010802FF"/>
            <UInt32 Value="70796"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="30"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.2.8.1.255-->
            <OctetString Value="0100020801FF"/>
            <UInt32 Value="6585"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="30"/>
            </Structure>
          </Structure>
          <Structure Qty="3">
            <!--1.0.2.8.2.255-->
            <OctetString Value="0100020802FF"/>
            <UInt32 Value="3851"/>
            <Structure Qty="2">
              <Int8 Value="0"/>
              <Enum Value="30"/>
            </Structure>
          </Structure>
        </Structure>
      </DataValue>
    </NotificationBody>
  </DataNotification>
</GatewayRequest></Messages>
```
I could take time to write the code to validate the HDLC frames, extract their data, parse them, etc but honestly, it annoys me a lot and it's not worth it when I simply want to know my current power consumption and with the results above, I see we can easily extract the wanted data from the raw stream just like that.

### Simple value extraction example

Here is a example of data extraction done in the code. The OBIS codes for **Active power+** and **Active power-**  and their related values are found in the stream below:

7EA8A4CF0223039996E6E7000F000002F10C07E808130114210A00FF88800210020209060000600100FF0908FFFFFFFFFFFFFFFFF020209060000600101FF0907FFFFFFFFFFFFFFFFF02030906`0100010700FF`06`00000000`02020F00161B02030906`0100020700FF`06`00000043`02020F00161B020309060100010800FF06000209DA02020F00161E020309060100020800FF06000028C402020F00161E02030906010020072FE07E7EA8A4CF022303999600FF12092C02020FFF1623020309060100340700FF12092302020FFF1623020309060100480700FF12093402020FFF16230203090601001F0700FF12000002020FFE1621020309060100330700FF12001202020FFE1621020309060100470700FF12004602020FFE1621020309060100010801FF060000F54E02020F00161E020309060100010802FF060001148C02020F00161E020309060100845B7E7EA02ECF022313BD61020801FF06000019B902020F00161E020309060100020802FF0600000F0B02020F00161E649A7E

Then a simple conversion to extract the uint32_t values and we get the following values, which correspond to what was parsed in the xml above by Gurux.

Active power+ = 0x00000000 -> 0W  
Active power- = 0x00000043 -> 67W

### Special case
The OBIS code 1.0.2.8.1.255 (0100020801FF) for example is not directly findable in the stream because it is splitted in two frames (the second and the third). But honestly its value is always located at the same location in the stream, thus if one wants to extract it's value, a simple look at the stream is enough to determine where is the value. Note the stream sent is always the same structure and length.  

Here is the it's value for the example.

7EA8A4CF0223039996E6E7000F000002F10C07E808130114210A00FF88800210020209060000600100FF0908FFFFFFFFFFFFFFFFF020209060000600101FF0907FFFFFFFFFFFFFFFFF020309060100010700FF060000000002020F00161B020309060100020700FF060000004302020F00161B020309060100010800FF06000209DA02020F00161E020309060100020800FF06000028C402020F00161E02030906010020072FE07E7EA8A4CF022303999600FF12092C02020FFF1623020309060100340700FF12092302020FFF1623020309060100480700FF12093402020FFF16230203090601001F0700FF12000002020FFE1621020309060100330700FF12001202020FFE1621020309060100470700FF12004602020FFE1621020309060100010801FF060000F54E02020F00161E020309060100010802FF060001148C02020F00161E020309060100845B7E7EA02ECF022313BD61020801FF06`000019B9`02020F00161E020309060100020802FF0600000F0B02020F00161E649A7E

## Pinout of the P1 connector
The pinout follows the DSMR 5.0 [standard](docs/SPEC%20-%20E-Meter_P1_specification_20210308.pdf) (p.12).

The important points are we get **5V 250mA** and **GND** pins, a **request data** pin and a **data** pin.  
It is necessary to output a high level on the data request pin to enable the data stream, 3.3V is enough, no need to have 5V if a 3.3V board is used. Also the data pin is an open colector, thus a pull-up is needed on the board side but no need to have a level translator when a 3.3V board is used. Another unspecified thing is that the data are inverted. Thus we need to invert the UART RX on the board.

# Note
I found some projects doing parts of what I needed on Github. But all were too complicated to use, integrate or simply understand. I like when things are simple and honestly I'm quite lazy. That's why I wrote this dead simple code.  
Honorable mention to this project https://github.com/Tropaion/ZigBee_SmartMeter_Reader which did for me the ZigBee part.
