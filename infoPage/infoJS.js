// reveal,
const observer = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      const card = entry.target;
      card.classList.add("active");

      card.addEventListener("mouseenter", () => {
        card.style.transitionDelay = "0s";
      });
      card.addEventListener("mouseleave", () => {
        card.style.transitionDelay = "0s";
      });

      observer.unobserve(card);
    }
  });
}, { threshold: 0.2 });

document.querySelectorAll(".reveal").forEach(el => observer.observe(el));
