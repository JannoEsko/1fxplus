#
# SoF2Plus Makefile
# Game and gametype modules
#
# GNU Make required
#
COMPILE_PLATFORM=$(shell uname | sed -e 's/_.*//' | tr '[:upper:]' '[:lower:]' | sed -e 's/\//_/g')
COMPILE_ARCH=$(shell uname -m | sed -e 's/i.86/x86/' | sed -e 's/^arm.*/arm/')

ifeq ($(COMPILE_PLATFORM),sunos)
  # Solaris uname and GNU uname differ
  COMPILE_ARCH=$(shell uname -p | sed -e 's/i.86/x86/')
endif

ifndef BUILD_GAME
  BUILD_GAME       =
endif
ifndef BUILD_GT_DM
  BUILD_GT_DM      =
endif
ifndef BUILD_GT_TDM
  BUILD_GT_TDM     =
endif
ifndef BUILD_GT_CTF
  BUILD_GT_CTF     =
endif
ifndef BUILD_GT_INF
  BUILD_GT_INF     =
endif
ifndef BUILD_GT_ELIM
  BUILD_GT_ELIM    =
endif
ifndef BUILD_GT_DEM
  BUILD_GT_DEM     =
endif
ifndef BUILD_GT_HNS
  BUILD_GT_HNS     =
endif
ifndef BUILD_GT_HNZ
  BUILD_GT_HNZ     =
endif

#############################################################################
#
# If you require a different configuration from the defaults below, create a
# new file named "Makefile.local" in the same directory as this file and define
# your parameters there. This allows you to change configuration without
# causing problems with keeping up to date with the repository.
#
#############################################################################
-include Makefile.local

ifeq ($(COMPILE_PLATFORM),cygwin)
  PLATFORM=mingw32
endif

ifndef PLATFORM
PLATFORM=$(COMPILE_PLATFORM)
endif
export PLATFORM

ifeq ($(PLATFORM),mingw32)
  MINGW=1
endif
ifeq ($(PLATFORM),mingw64)
  MINGW=1
endif

ifeq ($(COMPILE_ARCH),i86pc)
  COMPILE_ARCH=x86
endif

ifeq ($(COMPILE_ARCH),amd64)
  COMPILE_ARCH=x86_64
endif
ifeq ($(COMPILE_ARCH),x64)
  COMPILE_ARCH=x86_64
endif

ifeq ($(COMPILE_ARCH),powerpc)
  COMPILE_ARCH=ppc
endif
ifeq ($(COMPILE_ARCH),powerpc64)
  COMPILE_ARCH=ppc64
endif

ifeq ($(COMPILE_ARCH),axp)
  COMPILE_ARCH=alpha
endif

ifndef ARCH
ARCH=$(COMPILE_ARCH)
endif
export ARCH

ifneq ($(PLATFORM),$(COMPILE_PLATFORM))
  CROSS_COMPILING=1
else
  CROSS_COMPILING=0

  ifneq ($(ARCH),$(COMPILE_ARCH))
    CROSS_COMPILING=1
  endif
endif
export CROSS_COMPILING

ifndef VERSION
VERSION=0.01
endif

ifndef BASEGAME
BASEGAME=base
endif

ifndef COPYDIR
COPYDIR="/usr/local/games/sof2"
endif

ifndef COPYBINDIR
COPYBINDIR=$(COPYDIR)
endif

ifndef MOUNT_DIR
MOUNT_DIR=code
endif

ifndef BUILD_DIR
BUILD_DIR=build
endif

ifndef DEBUG_CFLAGS
DEBUG_CFLAGS=-ggdb -O0
endif

#############################################################################

BD=$(BUILD_DIR)/debug-$(PLATFORM)-$(ARCH)
BR=$(BUILD_DIR)/release-$(PLATFORM)-$(ARCH)
CMDIR=$(MOUNT_DIR)/qcommon
GDIR=$(MOUNT_DIR)/game
GTDIR=$(MOUNT_DIR)/gametype
GTDMDIR=$(MOUNT_DIR)/gametype/gt_dm
GTTDMDIR=$(MOUNT_DIR)/gametype/gt_tdm
GTCTFDIR=$(MOUNT_DIR)/gametype/gt_ctf
GTINFDIR=$(MOUNT_DIR)/gametype/gt_inf
GTELIMDIR=$(MOUNT_DIR)/gametype/gt_elim
GTDEMDIR=$(MOUNT_DIR)/gametype/gt_dem
GTHNSDIR=$(MOUNT_DIR)/gametype/gt_hns
GTHNZDIR=$(MOUNT_DIR)/gametype/gt_hnz

EXTDIR=$(MOUNT_DIR)/ext

bin_path=$(shell which $(1) 2> /dev/null)

# Add git version info
ifeq ($(wildcard .git),.git)
  GIT_REV=$(shell git show -s --pretty=format:%h-%ad --date=short)
  ifneq ($(GIT_REV),)
    VERSION:=$(VERSION)_GIT_$(GIT_REV)
  endif
endif


#############################################################################
# SETUP AND BUILD -- LINUX
#############################################################################

INSTALL=install
MKDIR=mkdir -p
EXTRA_FILES=

ifneq (,$(findstring "$(PLATFORM)", "linux" "gnu_kfreebsd" "kfreebsd-gnu" "gnu"))
  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -pipe -DARCH_STRING=\\\"$(ARCH)\\\" -pthread

  OPTIMIZE = -O3

  ifeq ($(ARCH),x86)
    OPTIMIZE = -O3 -march=i586
  else
  ifeq ($(ARCH),ppc)
    BASE_CFLAGS += -maltivec
  endif
  ifeq ($(ARCH),ppc64)
    BASE_CFLAGS += -maltivec
  endif
  ifeq ($(ARCH),sparc)
    OPTIMIZE += -mtune=ultrasparc3 -mv8plus
  endif
  endif
  
  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC -fvisibility=hidden
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  LIBS=-ldl -lm -pthread

  ifeq ($(ARCH),x86)
    # linux32 make ...
    BASE_CFLAGS += -m32
  else
  ifeq ($(ARCH),ppc64)
    BASE_CFLAGS += -m64
  endif
  endif
else # ifeq Linux

#############################################################################
# SETUP AND BUILD -- MAC OS X
#############################################################################

ifeq ($(PLATFORM),darwin)
  LIBS = -framework Cocoa
  OPTIMIZE = -O3

  # Default minimum Mac OS X version
  ifeq ($(MACOSX_VERSION_MIN),)
    MACOSX_VERSION_MIN=10.7
  endif

  MACOSX_MAJOR=$(shell echo $(MACOSX_VERSION_MIN) | cut -d. -f1)
  MACOSX_MINOR=$(shell echo $(MACOSX_VERSION_MIN) | cut -d. -f2)
  ifeq ($(shell test $(MACOSX_MINOR) -gt 9; echo $$?),0)
    # Multiply and then remove decimal. 10.10 -> 101000.0 -> 101000
    MAC_OS_X_VERSION_MIN_REQUIRED=$(shell echo "$(MACOSX_MAJOR) * 10000 + $(MACOSX_MINOR) * 100" | bc | cut -d. -f1)
  else
    # Multiply by 100 and then remove decimal. 10.7 -> 1070.0 -> 1070
    MAC_OS_X_VERSION_MIN_REQUIRED=$(shell echo "$(MACOSX_VERSION_MIN) * 100" | bc | cut -d. -f1)
  endif

  LDFLAGS += -mmacosx-version-min=$(MACOSX_VERSION_MIN)
  BASE_CFLAGS += -mmacosx-version-min=$(MACOSX_VERSION_MIN) \
                 -DMAC_OS_X_VERSION_MIN_REQUIRED=$(MAC_OS_X_VERSION_MIN_REQUIRED)

  ifeq ($(ARCH),ppc)
    BASE_CFLAGS += -arch ppc -faltivec
    OPTIMIZE += -O3
  endif
  ifeq ($(ARCH),ppc64)
    BASE_CFLAGS += -arch ppc64 -faltivec
  endif
  ifeq ($(ARCH),x86)
    OPTIMIZE += -march=prescott -mfpmath=sse
    # x86 vm will crash without -mstackrealign since MMX instructions will be
    # used no matter what and they corrupt the frame pointer in VM calls
    BASE_CFLAGS += -arch i386 -m32 -mstackrealign
  endif
  ifeq ($(ARCH),x86_64)
    OPTIMIZE += -mfpmath=sse
    BASE_CFLAGS += -arch x86_64
  endif

  # When compiling on OSX for OSX, we're not cross compiling as far as the
  # Makefile is concerned, as target architecture is specified as a compiler
  # argument
  ifeq ($(COMPILE_PLATFORM),darwin)
    CROSS_COMPILING=0
  endif

  ifeq ($(CROSS_COMPILING),1)
    ifeq ($(ARCH),x86_64)
      CC=x86_64-apple-darwin13-cc
    else
      ifeq ($(ARCH),x86)
        CC=i386-apple-darwin13-cc
      else
        $(error Architecture $(ARCH) is not supported when cross compiling)
      endif
    endif
  endif

  BASE_CFLAGS += -fno-strict-aliasing -fno-common -pipe -D_THREAD_SAFE=1
  SHLIBEXT=dylib
  SHLIBCFLAGS=-fPIC -fno-common
  SHLIBLDFLAGS=-dynamiclib $(LDFLAGS) -Wl,-U,_com_altivec

else # ifeq darwin


#############################################################################
# SETUP AND BUILD -- MINGW32
#############################################################################

ifdef MINGW

  ifeq ($(CROSS_COMPILING),1)
    # If CC is already set to something generic, we probably want to use
    # something more specific
    ifneq ($(findstring $(strip $(CC)),cc gcc),)
      CC=
    endif

    # We need to figure out the correct gcc
    ifeq ($(ARCH),x86_64)
      MINGW_PREFIXES=x86_64-w64-mingw32 amd64-mingw32msvc
    endif
    ifeq ($(ARCH),x86)
      MINGW_PREFIXES=i686-w64-mingw32 i586-mingw32msvc i686-pc-mingw32
    endif

    ifndef CC
      CC=$(firstword $(strip $(foreach MINGW_PREFIX, $(MINGW_PREFIXES), \
         $(call bin_path, $(MINGW_PREFIX)-gcc))))
    endif
  else
    # Some MinGW installations define CC to cc, but don't actually provide cc,
    # so check that CC points to a real binary and use gcc if it doesn't
    ifeq ($(call bin_path, $(CC)),)
      CC=gcc
    endif
  endif

  ifeq ($(CC),)
    $(error Cannot find a suitable cross compiler for $(PLATFORM))
  endif

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -DUSE_ICON -pthread

  ifeq ($(ARCH),x86_64)
    OPTIMIZE = -O3
  endif
  ifeq ($(ARCH),x86)
    OPTIMIZE = -O3 -march=i586
  endif

  SHLIBEXT=dll
  SHLIBCFLAGS=
  SHLIBLDFLAGS=-shared $(LDFLAGS)
  LIBS= -lws2_32 -lwinmm -lpsapi -pthread

  ifeq ($(ARCH),x86)
    # build 32bit
    BASE_CFLAGS += -m32
  else
    BASE_CFLAGS += -m64
  endif

else # ifdef MINGW

#############################################################################
# SETUP AND BUILD -- FREEBSD
#############################################################################

ifeq ($(PLATFORM),freebsd)

  # flags
  BASE_CFLAGS = \
    -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -DUSE_ICON -DMAP_ANONYMOUS=MAP_ANON -pthread
  OPTIMIZE =
  
  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  # don't need -ldl (FreeBSD)
  LIBS=-lm -pthread

  # cross-compiling tweaks
  ifeq ($(ARCH),x86)
    ifeq ($(CROSS_COMPILING),1)
      BASE_CFLAGS += -m32
    endif
  endif
  ifeq ($(ARCH),x86_64)
    ifeq ($(CROSS_COMPILING),1)
      BASE_CFLAGS += -m64
    endif
  endif

else # ifeq freebsd

#############################################################################
# SETUP AND BUILD -- OPENBSD
#############################################################################

ifeq ($(PLATFORM),openbsd)

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -pipe -DUSE_ICON -DMAP_ANONYMOUS=MAP_ANON -pthread

  OPTIMIZE = -O3

  ifeq ($(ARCH),x86)
    OPTIMIZE = -O3 -march=i586
  else
  ifeq ($(ARCH),ppc)
    BASE_CFLAGS += -maltivec
  endif
  ifeq ($(ARCH),ppc64)
    BASE_CFLAGS += -maltivec
  endif
  ifeq ($(ARCH),sparc64)
    OPTIMIZE += -mtune=ultrasparc3 -mv8plus
  endif
  endif
  
  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)

  LIBS=-lm -pthread

else # ifeq openbsd

#############################################################################
# SETUP AND BUILD -- NETBSD
#############################################################################

ifeq ($(PLATFORM),netbsd)

  LIBS=-lm -pthread
  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)
  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes -pthread

else # ifeq netbsd

#############################################################################
# SETUP AND BUILD -- IRIX
#############################################################################

ifeq ($(PLATFORM),irix64)
  LIB=lib
  ARCH=mips
  CC = c99

  BASE_CFLAGS=-Dstricmp=strcasecmp -Xcpluscomm -woff 1185 \
    -I. -I$(ROOT)/usr/include -pthread
  
  SHLIBEXT=so
  SHLIBCFLAGS=
  SHLIBLDFLAGS=-shared

  LIBS=-ldl -lm -lgen -pthread

else # ifeq IRIX

#############################################################################
# SETUP AND BUILD -- SunOS
#############################################################################

ifeq ($(PLATFORM),sunos)

  CC=gcc
  INSTALL=ginstall
  MKDIR=gmkdir -p
  COPYDIR="/usr/local/share/games/sof2"

  ifneq ($(ARCH),x86)
    ifneq ($(ARCH),sparc)
      $(error arch $(ARCH) is currently not supported)
    endif
  endif

  BASE_CFLAGS = -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes \
    -pipe -DUSE_ICON -pthread
  OPTIMIZE = -O3 -funroll-loops

  ifeq ($(ARCH),sparc)
    OPTIMIZE += \
      -fstrength-reduce -falign-functions=2 \
      -mtune=ultrasparc3 -mv8plus -mno-faster-structs
  ifeq ($(ARCH),x86)
    OPTIMIZE += -march=i586 -fomit-frame-pointer \
      -falign-functions=2 -fstrength-reduce
    BASE_CFLAGS += -m32
  endif
  endif

  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared $(LDFLAGS)
  LIBS=-lsocket -lnsl -ldl -lm -pthread

else # ifeq sunos

#############################################################################
# SETUP AND BUILD -- GENERIC
#############################################################################
  BASE_CFLAGS =
  
  SHLIBEXT=so
  SHLIBCFLAGS=-fPIC
  SHLIBLDFLAGS=-shared

endif #Linux
endif #darwin
endif #MINGW
endif #FreeBSD
endif #OpenBSD
endif #NetBSD
endif #IRIX
endif #SunOS

ifndef CC
  CC=gcc
endif

TARGETS =

ifndef SHLIBNAME
  SHLIBNAME=$(ARCH).$(SHLIBEXT)
endif

ifneq ($(BUILD_GAME),0)
  TARGETS += $(B)/sof2mp_game$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_DM),0)
  TARGETS += $(B)/gt_dm$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_TDM),0)
  TARGETS += $(B)/gt_tdm$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_CTF),0)
  TARGETS += $(B)/gt_ctf$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_INF),0)
  TARGETS += $(B)/gt_inf$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_ELIM),0)
  TARGETS += $(B)/gt_elim$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_DEM),0)
  TARGETS += $(B)/gt_dem$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_HNS),0)
  TARGETS += $(B)/gt_hns$(SHLIBNAME)
endif

ifneq ($(BUILD_GT_HNZ),0)
  TARGETS += $(B)/gt_hnz$(SHLIBNAME)
endif

ifeq ("$(CC)", $(findstring "$(CC)", "clang" "clang++"))
  BASE_CFLAGS += -Qunused-arguments
endif

ifdef DEFAULT_BASEDIR
  BASE_CFLAGS += -DDEFAULT_BASEDIR=\\\"$(DEFAULT_BASEDIR)\\\"
endif

ifeq ($(GENERATE_DEPENDENCIES),1)
  DEPEND_CFLAGS = -MMD
else
  DEPEND_CFLAGS =
endif

ifeq ($(NO_STRIP),1)
  STRIP_FLAG =
else
  STRIP_FLAG = -s
endif

# https://reproducible-builds.org/specs/source-date-epoch/
ifdef SOURCE_DATE_EPOCH
  BASE_CFLAGS += -DPRODUCT_DATE=\\\"$(shell date --date="@$$SOURCE_DATE_EPOCH" "+%b %_d %Y" | sed -e 's/ /\\\ /'g)\\\"
endif

BASE_CFLAGS += -DPRODUCT_VERSION=\\\"$(VERSION)\\\"
BASE_CFLAGS += -Wformat=2 -Wno-format-zero-length -Wformat-security -Wno-format-nonliteral
BASE_CFLAGS += -Wstrict-aliasing=2 -Wmissing-format-attribute
BASE_CFLAGS += -Wdisabled-optimization
BASE_CFLAGS += -Werror-implicit-function-declaration

ifeq ($(V),1)
echo_cmd=@:
Q=
else
echo_cmd=@echo
Q=@
endif

ifeq ($(GENERATE_DEPENDENCIES),1)
  DO_QVM_DEP=cat $(@:%.o=%.d) | sed -e 's/\.o/\.asm/g' >> $(@:%.o=%.d)
endif

define DO_GAME_CC
$(echo_cmd) "GAME_CC $<"
$(Q)$(CC) $(BASEGAME_CFLAGS) $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZE) -o $@ -c $<
endef

define DO_GT_CC
$(echo_cmd) "GT_CC $<"
$(Q)$(CC) $(BASEGAME_CFLAGS) $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZE) -o $@ -c $<
endef

define DO_SHLIB_CC
$(echo_cmd) "SHLIB_CC $<"
$(Q)$(CC) $(BASEGAME_CFLAGS) $(SHLIBCFLAGS) $(CFLAGS) $(OPTIMIZE) -o $@ -c $<
endef

#############################################################################
# MAIN TARGETS
#############################################################################

default: release
all: debug release

debug:
	@$(MAKE) targets B=$(BD) CFLAGS="$(CFLAGS) $(BASE_CFLAGS) $(DEPEND_CFLAGS) " \
	  OPTIMIZE="-D_DEBUG $(DEBUG_CFLAGS)" V=$(V)

release:
	@$(MAKE) targets B=$(BR) CFLAGS="$(CFLAGS) $(BASE_CFLAGS) $(DEPEND_CFLAGS)" \
	  OPTIMIZE="-DNDEBUG $(OPTIMIZE)" V=$(V)

ifneq ($(call bin_path, tput),)
  TERM_COLUMNS=$(shell if c=`tput cols`; then echo $$(($$c-4)); else echo 76; fi)
else
  TERM_COLUMNS=76
endif

define ADD_COPY_TARGET
TARGETS += $2
$2: $1
	$(echo_cmd) "CP $$<"
	@cp $1 $2
endef

# These functions allow us to generate rules for copying a list of files
# into the base directory of the build; this is useful for bundling libs,
# README files or whatever else
define GENERATE_COPY_TARGETS
$(foreach FILE,$1, \
  $(eval $(call ADD_COPY_TARGET, \
    $(FILE), \
    $(addprefix $(B)/,$(notdir $(FILE))))))
endef

$(call GENERATE_COPY_TARGETS,$(EXTRA_FILES))

NAKED_TARGETS=$(shell echo $(TARGETS) | sed -e "s!$(B)/!!g")

print_list=-@for i in $(1); \
     do \
             echo "    $$i"; \
     done

ifneq ($(call bin_path, fmt),)
  print_wrapped=@echo $(1) | fmt -w $(TERM_COLUMNS) | sed -e "s/^\(.*\)$$/    \1/"
else
  print_wrapped=$(print_list)
endif

# Create the build directories, check libraries and print out
# an informational message, then start building
targets: makedirs
	@echo ""
	@echo "Building in $(B):"
	@echo "  PLATFORM: $(PLATFORM)"
	@echo "  ARCH: $(ARCH)"
	@echo "  VERSION: $(VERSION)"
	@echo "  COMPILE_PLATFORM: $(COMPILE_PLATFORM)"
	@echo "  COMPILE_ARCH: $(COMPILE_ARCH)"
	@echo "  CC: $(CC)"
	@echo ""
	@echo "  CFLAGS:"
	$(call print_wrapped, $(CFLAGS) $(OPTIMIZE))
	@echo ""
	@echo "  LDFLAGS:"
	$(call print_wrapped, $(LDFLAGS))
	@echo ""
	@echo "  LIBS:"
	$(call print_wrapped, $(LIBS))
	@echo ""
	@echo "  Output:"
	$(call print_list, $(NAKED_TARGETS))
	@echo ""
ifneq ($(TARGETS),)
  ifndef DEBUG_MAKEFILE
	@$(MAKE) $(TARGETS) $(B).zip V=$(V)
  endif
endif

$(B).zip: $(TARGETS)
ifeq ($(PLATFORM),darwin)
  ifdef ARCHIVE
	@("./make-macosx-app.sh" release $(ARCH); if [ "$$?" -eq 0 ] && [ -d "$(B)/ioquake3.app" ]; then rm -f $@; cd $(B) && zip --symlinks -r9 ../../$@ `find "ioquake3.app" -print | sed -e "s!$(B)/!!g"`; else rm -f $@; cd $(B) && zip -r9 ../../$@ $(NAKED_TARGETS); fi)
  endif
endif
ifneq ($(PLATFORM),darwin)
  ifdef ARCHIVE
	@rm -f $@
	@(cd $(B) && zip -r9 ../../$@ $(NAKED_TARGETS))
  endif
endif

makedirs:
	@$(MKDIR) $(BUILD_DIR)
	@$(MKDIR) $(B)/ext
	@$(MKDIR) $(B)/ext/sqlite
	@$(MKDIR) $(B)/ext/rocmod
	@$(MKDIR) $(B)/game
	@$(MKDIR) $(B)/game/1fx
	@$(MKDIR) $(B)/gametype
	@$(MKDIR) $(B)/gametype/gt_dm
	@$(MKDIR) $(B)/gametype/gt_tdm
	@$(MKDIR) $(B)/gametype/gt_ctf
	@$(MKDIR) $(B)/gametype/gt_inf
	@$(MKDIR) $(B)/gametype/gt_elim
	@$(MKDIR) $(B)/gametype/gt_dem
	@$(MKDIR) $(B)/gametype/gt_hns
	@$(MKDIR) $(B)/gametype/gt_hnz
	@$(MKDIR) $(B)/qcommon

#############################################################################
# GAME MODULE
#############################################################################

SOF2GOBJ_ = \
  $(B)/game/g_main.o \
  \
  $(B)/game/ai_main.o \
  $(B)/game/ai_util.o \
  $(B)/game/ai_wpnav.o \
  \
  $(B)/game/bg_gametype.o \
  $(B)/game/bg_misc.o \
  $(B)/game/bg_player.o \
  $(B)/game/bg_pmove.o \
  $(B)/game/bg_slidemove.o \
  $(B)/game/bg_weapons.o \
  \
  $(B)/game/g_active.o \
  $(B)/game/g_antilag.o \
  $(B)/game/g_bot.o \
  $(B)/game/g_client.o \
  $(B)/game/g_cmds.o \
  $(B)/game/g_combat.o \
  $(B)/game/g_gametype.o \
  $(B)/game/g_items.o \
  $(B)/game/g_mem.o \
  $(B)/game/g_misc.o \
  $(B)/game/g_missile.o \
  $(B)/game/g_mover.o \
  $(B)/game/g_session.o \
  $(B)/game/g_spawn.o \
  $(B)/game/g_svcmds.o \
  $(B)/game/g_target.o \
  $(B)/game/g_team.o \
  $(B)/game/g_trigger.o \
  $(B)/game/g_utils.o \
  $(B)/game/g_weapon.o \
  $(B)/ext/sqlite/sqlite3.o \
  $(B)/ext/rocmod/rocmod.o \
  $(B)/game/1fx/logger.o \
  $(B)/game/1fx/database.o \
  $(B)/game/1fx/adminCommands.o \
  $(B)/game/1fx/1fxFunctions.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

SOF2GOBJ = $(SOF2GOBJ_) $(B)/game/g_syscalls.o

$(B)/sof2mp_game$(SHLIBNAME): $(SOF2GOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(SOF2GOBJ)

#############################################################################
# GT DM MODULE
#############################################################################

GTDMOBJ_ = \
  $(B)/gametype/gt_dm/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTDMOBJ = $(GTDMOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_dm$(SHLIBNAME): $(GTDMOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTDMOBJ)

#############################################################################
# GT TDM MODULE
#############################################################################

GTTDMOBJ_ = \
  $(B)/gametype/gt_tdm/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTTDMOBJ = $(GTTDMOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_tdm$(SHLIBNAME): $(GTTDMOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTTDMOBJ)

#############################################################################
# GT CTF MODULE
#############################################################################

GTCTFOBJ_ = \
  $(B)/gametype/gt_ctf/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTCTFOBJ = $(GTCTFOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_ctf$(SHLIBNAME): $(GTCTFOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTCTFOBJ)

#############################################################################
# GT INF MODULE
#############################################################################

GTINFOBJ_ = \
  $(B)/gametype/gt_inf/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTINFOBJ = $(GTINFOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_inf$(SHLIBNAME): $(GTINFOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTINFOBJ)

#############################################################################
# GT ELIM MODULE
#############################################################################

GTELIMOBJ_ = \
  $(B)/gametype/gt_elim/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTELIMOBJ = $(GTELIMOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_elim$(SHLIBNAME): $(GTELIMOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTELIMOBJ)

#############################################################################
# GT DEM MODULE
#############################################################################

GTDEMOBJ_ = \
  $(B)/gametype/gt_dem/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTDEMOBJ = $(GTDEMOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_dem$(SHLIBNAME): $(GTDEMOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTDEMOBJ)

#############################################################################
# GT hns MODULE
#############################################################################

GTHNSOBJ_ = \
  $(B)/gametype/gt_hns/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTHNSOBJ = $(GTHNSOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_hns$(SHLIBNAME): $(GTHNSOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTHNSOBJ)

#############################################################################
# GT hnz MODULE
#############################################################################

GTHNZOBJ_ = \
  $(B)/gametype/gt_hnz/gt_main.o \
  \
  $(B)/gametype/gt_shared.o \
  \
  $(B)/qcommon/q_math.o \
  $(B)/qcommon/q_shared.o

GTHNZOBJ = $(GTHNZOBJ_) $(B)/gametype/gt_syscalls.o

$(B)/gt_hnz$(SHLIBNAME): $(GTHNZOBJ)
	$(echo_cmd) "LD $@"
	$(Q)$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GTHNZOBJ)

#############################################################################
## MAIN RULES
#############################################################################

$(B)/qcommon/%.o: $(CMDIR)/%.c
	$(DO_SHLIB_CC)

$(B)/game/%.o: $(GDIR)/%.c
	$(DO_GAME_CC)
$(B)/ext/sqlite/%.o: $(EXTDIR)/sqlite/%.c
	$(DO_GAME_CC)
$(B)/ext/rocmod/%.o: $(EXTDIR)/rocmod/%.c
	$(DO_GAME_CC)
$(B)/game/1fx/%.o: $(GDIR)/1fx/%.c
	$(DO_GAME_CC)

$(B)/gametype/%.o: $(GTDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_dm/%.o: $(GTDMDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_tdm/%.o: $(GTTDMDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_ctf/%.o: $(GTCTFDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_inf/%.o: $(GTINFDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_elim/%.o: $(GTELIMDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_dem/%.o: $(GTDEMDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_hns/%.o: $(GTHNSDIR)/%.c
	$(DO_GT_CC)

$(B)/gametype/gt_hnz/%.o: $(GTHNZDIR)/%.c
	$(DO_GT_CC)

#############################################################################
# MISC
#############################################################################

copyfiles: release
	@if [ ! -d $(COPYDIR)/$(BASEGAME) ]; then echo "You need to set COPYDIR to where your SoF2 data is!"; fi

	$(INSTALL) $(STRIP_FLAG) -m 0755 $(BR)/sof2mp_game$(SHLIBNAME) $(COPYDIR)/$(BASEGAME)/.

clean: clean-debug clean-release

clean-debug:
	@$(MAKE) clean2 B=$(BD)

clean-release:
	@$(MAKE) clean2 B=$(BR)

clean2:
	@echo "CLEAN $(B)"
	@rm -f $(SOF2GOBJ)
	@rm -f $(GTDMOBJ)
	@rm -f $(GTTDMOBJ)
	@rm -f $(GTCTFOBJ)
	@rm -f $(GTINFOBJ)
	@rm -f $(GTELIMOBJ)
	@rm -f $(GTDEMOBJ)
	@rm -f $(GTHNSOBJ)
	@rm -f $(GTHNZOBJ)
	@rm -f $(TARGETS)

distclean: clean
	@rm -rf $(BUILD_DIR)

dist:
	git archive --format zip --output sof2mp_game-$(VERSION).zip HEAD

#############################################################################
# DEPENDENCIES
#############################################################################

.PHONY: all clean clean2 clean-debug clean-release copyfiles \
	debug default dist distclean makedirs \
	release targets

# If the target name contains "clean", don't do a parallel build
ifneq ($(findstring clean, $(MAKECMDGOALS)),)
.NOTPARALLEL:
endif
