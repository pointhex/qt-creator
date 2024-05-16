import qbs.FileInfo

QtcPlugin {
    name: "CppEditor"

    Depends { name: "Qt.widgets" }
    Depends { condition: project.withPluginTests; name: "Qt.testlib" }

    Depends { name: "CPlusPlus" }
    Depends { name: "Utils" }

    Depends { name: "Core" }
    Depends { name: "TextEditor" }
    Depends { name: "ProjectExplorer" }

    pluginTestDepends: [
        "QmakeProjectManager",
        "QbsProjectManager",
    ]

    cpp.defines: base
    Properties {
        condition: qbs.toolchain.contains("msvc")
        cpp.defines: base.concat("_SCL_SECURE_NO_WARNINGS")
    }

    files: [
        "abstracteditorsupport.cpp",
        "abstracteditorsupport.h",
        "baseeditordocumentparser.cpp",
        "baseeditordocumentparser.h",
        "baseeditordocumentprocessor.cpp",
        "baseeditordocumentprocessor.h",
        "builtincursorinfo.cpp",
        "builtincursorinfo.h",
        "builtineditordocumentparser.cpp",
        "builtineditordocumentparser.h",
        "builtineditordocumentprocessor.cpp",
        "builtineditordocumentprocessor.h",
        "clangdiagnosticconfig.cpp",
        "clangdiagnosticconfig.h",
        "clangdiagnosticconfigsmodel.cpp",
        "clangdiagnosticconfigsmodel.h",
        "clangdiagnosticconfigsselectionwidget.cpp",
        "clangdiagnosticconfigsselectionwidget.h",
        "clangdiagnosticconfigswidget.cpp",
        "clangdiagnosticconfigswidget.h",
        "clangdsettings.cpp",
        "clangdsettings.h",
        "compileroptionsbuilder.cpp",
        "compileroptionsbuilder.h",
        "cppautocompleter.cpp",
        "cppautocompleter.h",
        "cppbuiltinmodelmanagersupport.cpp",
        "cppbuiltinmodelmanagersupport.h",
        "cppcanonicalsymbol.cpp",
        "cppcanonicalsymbol.h",
        "cppchecksymbols.cpp",
        "cppchecksymbols.h",
        "cppcodeformatter.cpp",
        "cppcodeformatter.h",
        "cppcodemodelinspectordialog.cpp",
        "cppcodemodelinspectordialog.h",
        "cppcodemodelinspectordumper.cpp",
        "cppcodemodelinspectordumper.h",
        "cppcodemodelsettings.cpp",
        "cppcodemodelsettings.h",
        "cppcodestylepreferences.cpp",
        "cppcodestylepreferences.h",
        "cppcodestylepreferencesfactory.cpp",
        "cppcodestylepreferencesfactory.h",
        "cppcodestylesettings.cpp",
        "cppcodestylesettings.h",
        "cppcodestylesettingspage.cpp",
        "cppcodestylesettingspage.h",
        "cppcodestylesnippets.h",
        "cppcompletionassist.cpp",
        "cppcompletionassist.h",
        "cppcompletionassistprocessor.cpp",
        "cppcompletionassistprocessor.h",
        "cppcompletionassistprovider.cpp",
        "cppcompletionassistprovider.h",
        "cppcursorinfo.h",
        "cppdoxygen.cpp",
        "cppdoxygen.h",
        "cppdoxygen.kwgen",
        "cppeditorwidget.cpp",
        "cppeditorwidget.h",
        "cppeditor.qrc",
        "cppeditor_global.h",
        "cppeditortr.h",
        "cppeditorconstants.h",
        "cppeditordocument.cpp",
        "cppeditordocument.h",
        "cppeditoroutline.cpp",
        "cppeditoroutline.h",
        "cppeditorplugin.cpp",
        "cppelementevaluator.cpp",
        "cppelementevaluator.h",
        "cppfileiterationorder.cpp",
        "cppfileiterationorder.h",
        "cppfilesettingspage.cpp",
        "cppfilesettingspage.h",
        "cppfindreferences.cpp",
        "cppfindreferences.h",
        "cppfollowsymbolundercursor.cpp",
        "cppfollowsymbolundercursor.h",
        "cppfunctiondecldeflink.cpp",
        "cppfunctiondecldeflink.h",
        "cppfunctionparamrenaminghandler.cpp",
        "cppfunctionparamrenaminghandler.h",
        "cppheadersource.cpp",
        "cppheadersource.h",
        "cpphighlighter.cpp",
        "cpphighlighter.h",
        "cppincludehierarchy.cpp",
        "cppincludehierarchy.h",
        "cppincludesfilter.cpp",
        "cppincludesfilter.h",
        "cppindexingsupport.cpp",
        "cppindexingsupport.h",
        "cpplocalrenaming.cpp",
        "cpplocalrenaming.h",
        "cpplocalsymbols.cpp",
        "cpplocalsymbols.h",
        "cpplocatordata.cpp",
        "cpplocatordata.h",
        "cpplocatorfilter.cpp",
        "cpplocatorfilter.h",
        "cppmodelmanager.cpp",
        "cppmodelmanager.h",
        "cppmodelmanagersupport.cpp",
        "cppmodelmanagersupport.h",
        "cppoutline.cpp",
        "cppoutline.h",
        "cppoutlinemodel.cpp",
        "cppoutlinemodel.h",
        "cppparsecontext.cpp",
        "cppparsecontext.h",
        "cpppointerdeclarationformatter.cpp",
        "cpppointerdeclarationformatter.h",
        "cppprojectpartchooser.cpp",
        "cppprojectpartchooser.h",
        "cpppreprocessordialog.cpp",
        "cpppreprocessordialog.h",
        "cppprojectfile.cpp",
        "cppprojectfile.h",
        "cppprojectfilecategorizer.cpp",
        "cppprojectfilecategorizer.h",
        "cppprojectinfogenerator.cpp",
        "cppprojectinfogenerator.h",
        "cppprojectupdater.cpp",
        "cppprojectupdater.h",
        "cppqtstyleindenter.cpp",
        "cppqtstyleindenter.h",
        "cpprefactoringchanges.cpp",
        "cpprefactoringchanges.h",
        "cppselectionchanger.cpp",
        "cppselectionchanger.h",
        "cppsemanticinfo.h",
        "cppsemanticinfoupdater.cpp",
        "cppsemanticinfoupdater.h",
        "cppsourceprocessor.cpp",
        "cppsourceprocessor.h",
        "cpptoolsjsextension.cpp",
        "cpptoolsjsextension.h",
        "cpptoolsreuse.cpp",
        "cpptoolsreuse.h",
        "cpptoolssettings.cpp",
        "cpptoolssettings.h",
        "cpptypehierarchy.cpp",
        "cpptypehierarchy.h",
        "cppuseselectionsupdater.cpp",
        "cppuseselectionsupdater.h",
        "cppvirtualfunctionassistprovider.cpp",
        "cppvirtualfunctionassistprovider.h",
        "cppvirtualfunctionproposalitem.cpp",
        "cppvirtualfunctionproposalitem.h",
        "cppworkingcopy.cpp",
        "cppworkingcopy.h",
        "cursorineditor.h",
        "doxygengenerator.cpp",
        "doxygengenerator.h",
        "editordocumenthandle.cpp",
        "editordocumenthandle.h",
        "functionutils.cpp",
        "functionutils.h",
        "generatedcodemodelsupport.cpp",
        "generatedcodemodelsupport.h",
        "headerpathfilter.cpp",
        "headerpathfilter.h",
        "includeutils.cpp",
        "includeutils.h",
        "indexitem.cpp",
        "indexitem.h",
        "insertionpointlocator.cpp",
        "insertionpointlocator.h",
        "projectinfo.cpp",
        "projectinfo.h",
        "projectpart.cpp",
        "projectpart.h",
        "resourcepreviewhoverhandler.cpp",
        "resourcepreviewhoverhandler.h",
        "searchsymbols.cpp",
        "searchsymbols.h",
        "semantichighlighter.cpp",
        "semantichighlighter.h",
        "symbolfinder.cpp",
        "symbolfinder.h",
        "symbolsfindfilter.cpp",
        "symbolsfindfilter.h",
        "typehierarchybuilder.cpp",
        "typehierarchybuilder.h",
        "wrappablelineedit.cpp",
        "wrappablelineedit.h",
    ]

    Group {
        name: "Quickfixes"
        prefix: "quickfixes/"
        files: [
            "assigntolocalvariable.cpp",
            "assigntolocalvariable.h",
            "bringidentifierintoscope.cpp",
            "bringidentifierintoscope.h",
            "completeswitchstatement.cpp",
            "completeswitchstatement.h",
            "convertfromandtopointer.cpp",
            "convertfromandtopointer.h",
            "convertqt4connect.cpp",
            "convertqt4connect.h",
            "convertstringliteral.cpp",
            "convertstringliteral.h",
            "converttometamethodcall.cpp",
            "converttometamethodcall.h",
            "cppcodegenerationquickfixes.cpp",
            "cppcodegenerationquickfixes.h",
            "cppinsertvirtualmethods.cpp",
            "cppinsertvirtualmethods.h",
            "cppquickfix.cpp",
            "cppquickfix.h",
            "cppquickfixassistant.cpp",
            "cppquickfixassistant.h",
            "cppquickfixes.cpp",
            "cppquickfixes.h",
            "cppquickfixhelpers.cpp",
            "cppquickfixhelpers.h",
            "cppquickfixprojectsettings.cpp",
            "cppquickfixprojectsettings.h",
            "cppquickfixprojectsettingswidget.cpp",
            "cppquickfixprojectsettingswidget.h",
            "cppquickfixsettings.cpp",
            "cppquickfixsettings.h",
            "cppquickfixsettingspage.cpp",
            "cppquickfixsettingspage.h",
            "cppquickfixsettingswidget.cpp",
            "cppquickfixsettingswidget.h",
            "createdeclarationfromuse.cpp",
            "createdeclarationfromuse.h",
            "extractfunction.cpp",
            "extractfunction.h",
            "extractliteralasparameter.cpp",
            "extractliteralasparameter.h",
            "insertfunctiondefinition.cpp",
            "insertfunctiondefinition.h",
            "logicaloperationquickfixes.cpp",
            "logicaloperationquickfixes.h",
            "moveclasstoownfile.cpp",
            "moveclasstoownfile.h",
            "movefunctiondefinition.cpp",
            "movefunctiondefinition.h",
            "removeusingnamespace.cpp",
            "removeusingnamespace.h",
            "rewritecomment.cpp",
            "rewritecomment.h",
            "rewritecontrolstatements.cpp",
            "rewritecontrolstatements.h",
            "splitsimpledeclaration.cpp",
            "splitsimpledeclaration.h",
        ]
    }

    Group {
        name: "TestCase"
        condition: qtc.withPluginTests || qtc.withAutotests
        files: [
            "cpptoolstestcase.cpp",
            "cpptoolstestcase.h",
        ]
    }

    QtcTestFiles {
        cpp.defines: outer.concat(['SRCDIR="' + FileInfo.path(filePath) + '"'])

        Group {
            name: "Quickfix tests"
            prefix: "quickfixes/"
            files: [
                "cppquickfix_test.cpp",
                "cppquickfix_test.h",
            ]
        }

        files: [
            "compileroptionsbuilder_test.cpp",
            "compileroptionsbuilder_test.h",
            "cppcodegen_test.cpp",
            "cppcodegen_test.h",
            "cppcompletion_test.cpp",
            "cppcompletion_test.h",
            "cppdoxygen_test.cpp",
            "cppdoxygen_test.h",
            "cppincludehierarchy_test.cpp",
            "cppincludehierarchy_test.h",
            "cpplocalsymbols_test.cpp",
            "cpplocalsymbols_test.h",
            "cpplocatorfilter_test.cpp",
            "cpplocatorfilter_test.h",
            "cppmodelmanager_test.cpp",
            "cppmodelmanager_test.h",
            "cpppointerdeclarationformatter_test.cpp",
            "cpppointerdeclarationformatter_test.h",
            "cpprenaming_test.cpp",
            "cpprenaming_test.h",
            "cppsourceprocessor_test.cpp",
            "cppsourceprocessor_test.h",
            "cppsourceprocessertesthelper.cpp",
            "cppsourceprocessertesthelper.h",
            "cppuseselections_test.cpp",
            "cppuseselections_test.h",
            "fileandtokenactions_test.cpp",
            "fileandtokenactions_test.h",
            "followsymbol_switchmethoddecldef_test.cpp",
            "followsymbol_switchmethoddecldef_test.h",
            "modelmanagertesthelper.cpp",
            "modelmanagertesthelper.h",
            "projectinfo_test.cpp",
            "projectinfo_test.h",
            "symbolsearcher_test.cpp",
            "symbolsearcher_test.h",
            "typehierarchybuilder_test.cpp",
            "typehierarchybuilder_test.h",
        ]
    }

    Export {
        Depends { name: "CPlusPlus" }
        Depends { name: "Qt.concurrent" }
    }
}
