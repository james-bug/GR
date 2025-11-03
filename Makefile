# Copyright (C) 2025 Gaming System Project
# This is free software, licensed under the GPL-2.0

include $(TOPDIR)/rules.mk

PKG_NAME:=gaming-core
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_MAINTAINER:=Gaming System Team
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk


define Package/gaming-core
  SECTION:=BenQ
  CATEGORY:=BenQ
  TITLE:=Gaming System Core Library
  SUBMENU:=Applications
  DEPENDS:=+libc +libuci +libubox +libubus
endef


define Package/gaming-core/description
  Gaming System Core Library provides hardware abstraction layer (HAL),
  GPIO control, LED controller, ADC reader, logger, config parser,
  and socket helper for gaming-client and gaming-server packages.
  
  Features:
  - HAL (Hardware Abstraction Layer)
  - GPIO Library with interrupt support
  - RGB LED Controller (PWM)
  - ADC Reader for device type detection
  - Logger (syslog integration)
  - UCI Config Parser
  - Socket Helper (Unix/TCP)
  - Unified Init Script with device detection
endef

define Package/gaming-core/conffiles
/etc/config/gaming
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-fPIC -shared \
		-I$(PKG_BUILD_DIR) \
		-I$(PKG_BUILD_DIR)/hal \
		$(PKG_BUILD_DIR)/hal/hal_init.c \
		$(PKG_BUILD_DIR)/hal/hal_real.c \
		$(PKG_BUILD_DIR)/gpio_lib.c \
		$(PKG_BUILD_DIR)/led_controller.c \
		$(PKG_BUILD_DIR)/adc_reader.c \
		$(PKG_BUILD_DIR)/logger.c \
		$(PKG_BUILD_DIR)/config_parser.c \
		$(PKG_BUILD_DIR)/socket_helper.c \
		-o $(PKG_BUILD_DIR)/libgaming-core.so \
		-luci -lubox -lubus
endef

define Package/gaming-core/install
	# 安裝共享庫
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libgaming-core.so $(1)/usr/lib/
	
	# 安裝標頭檔 (供其他套件使用)
	$(INSTALL_DIR) $(1)/usr/include/gaming
	$(INSTALL_DIR) $(1)/usr/include/gaming/hal
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/gaming_common.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/hal_interface.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/gpio_lib.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/led_controller.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/adc_reader.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/logger.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/config_parser.h $(1)/usr/include/gaming/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/socket_helper.h $(1)/usr/include/gaming/
	
	# 安裝 Init Script (Phase 2)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/gaming $(1)/etc/init.d/gaming
	
	# 安裝配置檔 (Phase 2)
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/etc/config/gaming $(1)/etc/config/gaming
	
endef

define Package/gaming-core/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	echo "Gaming Core installed successfully!"
	echo ""
	echo "Configuration:"
	echo "  1. Set device type (choose one method):"
	echo "     - Quick cache: echo 'client' > /var/run/gaming_device_type"
	echo "     - UCI config: uci set gaming.core.device_type=client"
	echo ""
	echo "  2. Enable and start service:"
	echo "     /etc/init.d/gaming enable"
	echo "     /etc/init.d/gaming start"
	echo ""
	echo "Note: Replace 'client' with 'server' for server device"
}
endef

define Package/gaming-core/prerm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	/etc/init.d/gaming stop
	/etc/init.d/gaming disable
}
endef

$(eval $(call BuildPackage,gaming-core))
