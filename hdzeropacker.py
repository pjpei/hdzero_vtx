FILENAME = "dm5680_fw_tx_race.hex.bin"

fileKeil = open(FILENAME, "rb")
keilContent = fileKeil.read()

print("Flipping bits...")
bitFlipDictionary = {
    0x01 : 0x80,
    0x02 : 0x04,
    0x04 : 0x20,
    0x08 : 0x02,
    0x10 : 0x08,
    0x20 : 0x10,
    0x40 : 0x01,
    0x80 : 0x40
}


byteMap = {}
reverseByteMap = {}

for i in range(256):
    newByte = 0
    for bit in bitFlipDictionary:
        if(i & bit == bit):
            newByte = newByte + bitFlipDictionary[bit]
    byteMap[i] = newByte
    reverseByteMap[newByte] = i


byteArr = []
reverseByteArr = []

garbageDay = open("HDZERO_NEW_TX.bin", "wb")
garbageSource = open("HDZERO_BITUNFLIPPED.bin", "wb")

for sourceByte in keilContent:
    byteArr.append(byteMap[sourceByte])

for destinationByte in hdzeroContent:
    reverseByteArr.append(reverseByteMap[destinationByte])

garbageDay.write(bytearray(byteArr))
garbageSource.write(bytearray(reverseByteArr))
print("Done.")