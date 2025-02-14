include $(TOPDIR)/rules.mk

PKG_NAME:=telematics-gateway
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/balaji-balu/telematics-gateway.git
PKG_SOURCE_VERSION:=main
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/telematics-gateway
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Telematics Gateway
  DEPENDS:=+libmosquitto +libcoap
endef

define Package/telematics-gateway/description
  A telematics gateway application supporting MQTT and CoAP for OpenWRT.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/telematics-gateway/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/telematics-gateway $(1)/usr/bin/
endef

$(eval $(call BuildPackage,telematics-gateway))
