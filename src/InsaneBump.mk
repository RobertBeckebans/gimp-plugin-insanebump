##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=InsaneBump
ConfigurationName      :=Release
WorkspacePath          := "C:\Documents and Settings\Derby Russell\My Documents\CodeLite\Projects\gimp-plugin-insanebump\linux\gimp-plugin-insanebump\src"
ProjectPath            := "C:\Documents and Settings\Derby Russell\My Documents\CodeLite\Projects\gimp-plugin-insanebump\linux\gimp-plugin-insanebump\src"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Derby Russell
Date                   :=12/7/2013
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=gcc
SharedObjectLinkerName :=gcc -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="C:\Documents and Settings\Derby Russell\My Documents\CodeLite\Projects\gimp-plugin-insanebump\linux\gimp-plugin-insanebump\src\InsaneBump.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
LinkOptions            :=  -lgimp.dll -lglib-2.0.dll -lgdk-win32-2.0.dll -lgtk-win32-2.0.dll -lgobject-2.0.dll -lgimpui.dll -lgimpbase.dll -lgimpwidgets.dll -lintl.dll -mwindows
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/gtk/include/cairo $(IncludeSwitch)C:/gtk/lib/gtk-2.0/include $(IncludeSwitch)C:/gtk/include/glib-2.0 $(IncludeSwitch)C:/gtk/include/atk-1.0 $(IncludeSwitch)C:/gtk/include/gtk-2.0 $(IncludeSwitch)C:/gtk/include $(IncludeSwitch)C:/gtk/include/gdk-pixbuf-2.0 $(IncludeSwitch)C:/gtk/include/pango-1.0 $(IncludeSwitch)C:/gtk/lib/glib-2.0/include $(IncludeSwitch)C:/gtk/include/freetype2 $(IncludeSwitch)C:/gtk/include/libpng14 $(IncludeSwitch)C:/gtk/include/gtkglext-1.0 $(IncludeSwitch)C:/gtk/lib/gtkglext-1.0/include $(IncludeSwitch)C:/gtk/include/gimp-2.0 $(IncludeSwitch)C:/GtkGLExt/1.0/lib/gtkglext-1.0/include $(IncludeSwitch)win32c 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)C:/gtk/lib/gimp-2.0 $(LibraryPathSwitch)C:/gtk/lib $(LibraryPathSwitch)C:/glew-1.10.0/lib 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := gcc
CC       := gcc
CXXFLAGS :=  -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
Objects=$(IntermediateDirectory)/scale$(ObjectSuffix) $(IntermediateDirectory)/InsaneBump$(ObjectSuffix) $(IntermediateDirectory)/interface$(ObjectSuffix) $(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/normalmap$(ObjectSuffix) $(IntermediateDirectory)/PluginConnectors$(ObjectSuffix) $(IntermediateDirectory)/preview_area$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/scale$(ObjectSuffix): scale.c $(IntermediateDirectory)/scale$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/scale.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/scale$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scale$(DependSuffix): scale.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/scale$(ObjectSuffix) -MF$(IntermediateDirectory)/scale$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/scale.c"

$(IntermediateDirectory)/scale$(PreprocessSuffix): scale.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/scale$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/scale.c"

$(IntermediateDirectory)/InsaneBump$(ObjectSuffix): InsaneBump.c $(IntermediateDirectory)/InsaneBump$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/InsaneBump.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/InsaneBump$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/InsaneBump$(DependSuffix): InsaneBump.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/InsaneBump$(ObjectSuffix) -MF$(IntermediateDirectory)/InsaneBump$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/InsaneBump.c"

$(IntermediateDirectory)/InsaneBump$(PreprocessSuffix): InsaneBump.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/InsaneBump$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/InsaneBump.c"

$(IntermediateDirectory)/interface$(ObjectSuffix): interface.c $(IntermediateDirectory)/interface$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/interface.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/interface$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/interface$(DependSuffix): interface.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/interface$(ObjectSuffix) -MF$(IntermediateDirectory)/interface$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/interface.c"

$(IntermediateDirectory)/interface$(PreprocessSuffix): interface.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/interface$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/interface.c"

$(IntermediateDirectory)/main$(ObjectSuffix): main.c $(IntermediateDirectory)/main$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/main.c"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/main.c"

$(IntermediateDirectory)/normalmap$(ObjectSuffix): normalmap.c $(IntermediateDirectory)/normalmap$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/normalmap.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/normalmap$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/normalmap$(DependSuffix): normalmap.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/normalmap$(ObjectSuffix) -MF$(IntermediateDirectory)/normalmap$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/normalmap.c"

$(IntermediateDirectory)/normalmap$(PreprocessSuffix): normalmap.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/normalmap$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/normalmap.c"

$(IntermediateDirectory)/PluginConnectors$(ObjectSuffix): PluginConnectors.c $(IntermediateDirectory)/PluginConnectors$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/PluginConnectors.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PluginConnectors$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PluginConnectors$(DependSuffix): PluginConnectors.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PluginConnectors$(ObjectSuffix) -MF$(IntermediateDirectory)/PluginConnectors$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/PluginConnectors.c"

$(IntermediateDirectory)/PluginConnectors$(PreprocessSuffix): PluginConnectors.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PluginConnectors$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/PluginConnectors.c"

$(IntermediateDirectory)/preview_area$(ObjectSuffix): preview_area.c $(IntermediateDirectory)/preview_area$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/preview_area.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/preview_area$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/preview_area$(DependSuffix): preview_area.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/preview_area$(ObjectSuffix) -MF$(IntermediateDirectory)/preview_area$(DependSuffix) -MM "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/preview_area.c"

$(IntermediateDirectory)/preview_area$(PreprocessSuffix): preview_area.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/preview_area$(PreprocessSuffix) "C:/Documents and Settings/Derby Russell/My Documents/CodeLite/Projects/gimp-plugin-insanebump/linux/gimp-plugin-insanebump/src/preview_area.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/scale$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/scale$(DependSuffix)
	$(RM) $(IntermediateDirectory)/scale$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/InsaneBump$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/InsaneBump$(DependSuffix)
	$(RM) $(IntermediateDirectory)/InsaneBump$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/interface$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/interface$(DependSuffix)
	$(RM) $(IntermediateDirectory)/interface$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/normalmap$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/normalmap$(DependSuffix)
	$(RM) $(IntermediateDirectory)/normalmap$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/PluginConnectors$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/PluginConnectors$(DependSuffix)
	$(RM) $(IntermediateDirectory)/PluginConnectors$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/preview_area$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/preview_area$(DependSuffix)
	$(RM) $(IntermediateDirectory)/preview_area$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "C:\Documents and Settings\Derby Russell\My Documents\CodeLite\Projects\gimp-plugin-insanebump\linux\gimp-plugin-insanebump\src\.build-release\InsaneBump"


