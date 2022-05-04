ifneq ($(wildcard vendor/lineage/.),)
PRODUCT_MAKEFILES := $(LOCAL_DIR)/lineage_hennessy.mk
endif

COMMON_LUNCH_CHOICES := \
    lineage_hennessy-eng \
    lineage_hennessy-user \
    lineage_hennessy-userdebug



