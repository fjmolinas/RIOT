# Install ARM GNU Embedded toolchain
ARM_GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm"
ARM_GCC_VERSION="9-2019q4"
ARM_GCC_ARCHIVE="gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2"
ARM_GCC_ARCHIVE_URL="${ARM_GCC_URL}/${ARM_GCC_VERSION}/${ARM_GCC_ARCHIVE}"

cd /opt && wget -nv -O - "${ARM_GCC_ARCHIVE_URL}" | tar -jxf -

echo "export PATH=/opt/gcc-arm-none-eabi-9-2019-q4-major/bin:\$PATH" >> /home/${SSH_USERNAME}/.bashrc

# Install MIPS toolchain
MIPS_VERSION=2020.06-01
curl -L "https://codescape.mips.com/components/toolchain/${MIPS_VERSION}/Codescape.GNU.Tools.Package.${MIPS_VERSION}.for.MIPS.MTI.Bare.Metal.CentOS-6.x86_64.tar.gz" -o - \
    | tar -C /opt -zx
rm -rf /opt/mips-mti-elf/*/share/{doc,info,man,locale}
cd /opt/mips-mti-elf/*/mips-mti-elf/bin
for f in *; do test -f "../../bin/mips-mti-elf-$f" && ln -f "../../bin/mips-mti-elf-$f" "$f"; done
cd -

echo "export MIPS_ELF_ROOT=/opt/mips-mti-elf/${MIPS_VERSION}" >> /home/${SSH_USERNAME}/.bashrc
echo "export PATH=\$MIPS_ELF_ROOT/bin:\$PATH" >> /home/${SSH_USERNAME}/.bashrc

# Install MSP430 toolchain
RIOT_TOOLCHAIN_GCC_VERSION=10.1.0
RIOT_TOOLCHAIN_PACKAGE_VERSION=18
RIOT_TOOLCHAIN_TAG=20200722112854-64162e7
RIOT_TOOLCHAIN_GCCPKGVER=${RIOT_TOOLCHAIN_GCC_VERSION}-${RIOT_TOOLCHAIN_PACKAGE_VERSION}
RIOT_TOOLCHAIN_SUBDIR=${RIOT_TOOLCHAIN_GCCPKGVER}-${RIOT_TOOLCHAIN_TAG}
MSP430_URL=https://github.com/RIOT-OS/toolchains/releases/download/${RIOT_TOOLCHAIN_SUBDIR}/riot-msp430-elf-${RIOT_TOOLCHAIN_GCCPKGVER}.tgz
get -q ${MSP430_URL} -O- | tar -C /opt -xz
echo "export PATH=\$PATH:/opt/riot-toolchain/msp430-elf/${RIOT_TOOLCHAIN_GCCPKGVER}/bin" >> /home/${SSH_USERNAME}/.bashrc

# Install RISC-V
RISCV_VERSION=10.1.0-1.1
wget -q https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases/download/v${RISCV_VERSION}/xpack-riscv-none-embed-gcc-${RISCV_VERSION}-linux-x64.tar.gz -O- \
        | tar -C /opt -xz
m -rf /opt/xpack-riscv-none-embed-gcc-${RISCV_VERSION}/share/doc
cd /opt/gnu-mcu-eclipse/riscv-none-gcc/*/riscv-none-embed/bin
for f in *; do test -f "../../bin/riscv-none-embed-$f" && \
    ln -f "../../bin/riscv-none-embed-$f" "$f"; \
done
cd -

echo "export PATH=/opt/xpack-riscv-none-embed-gcc-${RISCV_VERSION}/bin:\$PATH" >> /home/${SSH_USERNAME}/.bashrc

# Install ESP32 toolchain
echo 'Installing ESP32 toolchain'
mkdir -p /opt/esp
cd /opt/esp
git clone https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout -q f198339ec09e90666150672884535802304d23ec
git submodule update --init --recursive
rm -rf .git* docs examples make tools
rm -f add_path.sh CONTRIBUTING.rst Kconfig Kconfig.compiler
cd components
rm -rf app_trace app_update aws_iot bootloader bt coap console cxx \
        esp_adc_cal espcoredump esp_http_client esp-tls expat fatfs \
        freertos idf_test jsmn json libsodium log lwip mbedtls mdns \
        micro-ecc nghttp openssl partition_table pthread sdmmc spiffs \
        tcpip_adapter ulp vfs wear_levelling xtensa-debug-module && \
find . -name '*.[csS]' -exec rm {} \;
cd /opt/esp
git clone https://github.com/gschorcht/xtensa-esp32-elf.git && \
cd xtensa-esp32-elf && \
git checkout -q 414d1f3a577702e927973bd906357ee00d7a6c6c

echo "export PATH=/opt/esp/xtensa-esp32-elf/bin:\$PATH" >> /home/${SSH_USERNAME}/.bashrc

# Install ESP8266 toolchain
# Install complete ESP8266 toolchain in /opt/esp (146 MB after cleanup)
echo 'Installing ESP8266 toolchain' >&2 && \
    cd /opt && \
    git clone https://github.com/gschorcht/RIOT-Xtensa-ESP8266-toolchain.git esp && \
    cd esp && \
    git checkout -q df38b06 && \
    rm -rf .git

echo "export PATH=\$PATH:/opt/esp/esp-open-sdk/xtensa-lx106-elf/bin" >> /home/${SSH_USERNAME}/.bashrc
echo "export ESP8266_SDK_DIR=/opt/esp/esp-open-sdk/sdk" >> /home/${SSH_USERNAME}/.bashrc
echo "export ESP8266_NEWLIB_DIR=/opt/esp/newlib-xtensa" >> /home/${SSH_USERNAME}/.bashrc

# Install complete ESP8266 toolchain in /opt/esp (125 MB after cleanup)
# remember https://github.com/RIOT-OS/RIOT/pull/10801 when updating
# NOTE: We install the toolchain for the RTOS SDK in parallel in the first
# step and remove the old version as soon as the RIOT port for the ESP8266
# RTOS SDK has been merged.
echo 'Installing ESP8266 toolchain' >&2 && \
    mkdir -p /opt/esp && \
    cd /opt/esp && \
    git clone https://github.com/gschorcht/xtensa-esp8266-elf && \
    cd xtensa-esp8266-elf && \
    git checkout -q 696257c2b43e2a107d3108b2c1ca6d5df3fb1a6f && \
    rm -rf .git

echo "export PATH=\$PATH:/opt/esp/xtensa-esp8266-elf/bin" >> /home/${SSH_USERNAME}/.bashrc

#
