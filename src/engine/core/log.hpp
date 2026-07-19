// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// log — un petit système de journalisation (afficher des messages).
//
// Au lieu d'écrire des std::cout partout, on passe par cinder::log::info(...),
// warning(...) ou error(...). Avantage : un seul endroit décide COMMENT les
// messages sont affichés (ici on délègue à SDL, voir log.cpp), et on peut
// formater le texte facilement, comme printf mais en plus sûr.
//
// Exemple : cinder::log::error("Échec du chargement : {}", nom_fichier);
// ============================================================================

#ifndef CINDER_CITY_LOG_HPP
#define CINDER_CITY_LOG_HPP

#include <format>    // std::format : construit une chaîne à partir d'un modèle "{}"
#include <string>
#include <utility>   // std::forward

// Un "namespace" est un tiroir de rangement pour éviter les collisions de noms.
// Ici tout est rangé sous cinder::log (double niveau : cinder, puis log).
namespace cinder::log {
    // "enum class" = une liste de valeurs nommées, fortement typées.
    // Les trois niveaux d'importance d'un message.
    enum class level {
        info,      // information normale
        warning,   // avertissement (quelque chose d'inhabituel mais non bloquant)
        error      // erreur
    };

    // "detail" contient ce qui est interne : on ne l'appelle pas directement.
    namespace detail {
        // La vraie fonction d'écriture (implémentée dans log.cpp).
        // Elle reçoit un niveau + un texte déjà formaté.
        void write(level, std::string const&);
    }

    // Ci-dessous, trois fonctions "template". Un template est un patron : le
    // compilateur génère une version adaptée aux types d'arguments donnés.
    // "typename... Args" veut dire "un nombre quelconque d'arguments de types
    // quelconques" (variadique) — comme printf qui accepte autant d'arguments
    // qu'il y a de "{}" dans le texte.

    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        // std::format assemble le texte final ("Score : {}" + 42 -> "Score : 42").
        // std::forward transmet les arguments efficacement, sans copie inutile.
        detail::write(level::info, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warning(std::format_string<Args...> fmt, Args&&... args) {
        detail::write(level::warning, std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        detail::write(level::error, std::format(fmt, std::forward<Args>(args)...));
    }
}

#endif //CINDER_CITY_LOG_HPP
