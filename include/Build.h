//
// Created by richard on 29/01/23.
//

#ifndef ROSE2_BUILD_H
#define ROSE2_BUILD_H

namespace rose {
    /**
     * @brief Terminal function for parameter pack processing.
     * @tparam GadgetType the type of the Gadget being configured.
     */
    template<class GadgetType>
    void SetGadgetParameters(std::shared_ptr<GadgetType>&) {
    }

    /**
     * @brief Process the next parameter in the parameter pack.
     * @details This version processes an Enum tagged parameter. The Enum tagging helps to determine which field
     * is going to be set with the value of the parameter.
     * @tparam GadgetType the type of the Gadget being configured.
     * @tparam M The MetaType member which helps determine how to set this parameter on the Gadget.
     * @tparam Arg The type of the argument in the parameter
     * @tparam Rest The remainder of the parameter pack.
     * @param gadget The Gadget being configured.
     * @param arg The argument value to set on the Gadget.
     * @param rest The remaining values in the parameter pack.
     */
//    template<class GadgetType, class Arg, class...Rest>
//    void SetGadgetParameters(std::shared_ptr<GadgetType>& gadget, Arg arg, Rest...rest) {
//        setEnumParameter(gadget, arg);
//        SetGadgetParameters(gadget, rest...);
//    }

    /**
     * @brief Process the next parameter in the parameter pack.
     * @details This version sets a parameter which can be associated with the destination field based on
     * its type.
     * @tparam GadgetType The type of the Gadget being configured.
     * @tparam Arg The type of the parameter.
     * @tparam Rest The type of the remainder of the parameter pack.
     * @param gadget The Gadget being configured.
     * @param arg The parameter value.
     * @param rest The values of the rest of the parameter pack.
     */
    template<class GadgetType, class Arg, class...Rest>
    void SetGadgetParameters(std::shared_ptr<GadgetType>& gadget, Arg arg, Rest...rest) {
        setParameter(gadget, arg);
        SetGadgetParameters(gadget, rest...);
    }

    /**
     * @brief Build a Gadget using the provided parameters.
     * @details Build a Gadget using a variadic list of parameters. The desired Theme object must be the first
     * parameter. The remainder of the parameters must be passed in a way which allows identification of which
     * specialization of Gadget and field will be set with the parameter value. There are two ways to do this:
     *  - Specialization unique parameter types. When a specialization defines a unique type for one of its
     *  fields that type can be used to determine how the parameter should be used.
     *  - Enumeration guided resolution. When a type is not unique to a specialization or field it may be tagged
     *  with a rose::MetaType value to assist with the resolution.
     * @tparam GadgetType The type of Gadget to build.
     * @tparam Args The type of the parameter pack used to build the Gadget.
     * @param theme A std::shared_ptr to the Theme which must be the first argument to the Build method.
     * @param args Values of the remaining parameters used to configure the Gadget.
     * @return std::shared_ptr<GadgetType> if the Gadget is created, nullptr otherwise.
     */
    template<class GadgetType, class...Args>
    std::shared_ptr<GadgetType> Build(std::shared_ptr<rose::Theme>& theme, Args...args) {
        if (auto gadget = std::make_shared<GadgetType>(theme); gadget) {
            gadget->completeCompositeConstruction(theme);
            SetGadgetParameters(gadget, args...);
            return gadget;
        }
        return nullptr;
    }

    /**
     * @brief Set a gadget name on a rose::Gadget.
     * @details This version sets the Gadget name to a std::string value.
     * @param gadget the Gadget pointer.
     * @param parameter the parameter value.
     */
    template<class GadgetType>
    [[maybe_unused]] void setParameter(std::shared_ptr<GadgetType>& gadget, param::GadgetName parameter) {
        gadget->setName(parameter.data);
    }

    /**
     * @brief Set background Color value on a rose::Gadget.
     * @details This version sets the Gadget background color to a rose::Color value.
     * @tparam Parm Parameter<MetaType::Background,Color>
     * @param gadget the Gadget pointer.
     * @param parameter the parameter value.
     */
    template<class GadgetType>
    [[maybe_unused]] void setParameter(std::shared_ptr<GadgetType>& gadget, param::Background parameter) {
        gadget->setBackground(parameter.data);
    }


} // rose

#endif //ROSE2_BUILD_H
